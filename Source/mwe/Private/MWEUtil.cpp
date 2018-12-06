#include "MWEUtil.h"
#include "Engine.h"

FTransform UMWEUtil::FindNewRotation(FTransform A, FVector BDir)
{
	FQuat AQuat = A.GetRotation();
	FVector ADir = AQuat.GetForwardVector();

	ADir.Normalize();
	BDir.Normalize();

	FVector AxisOfRotation = FVector::CrossProduct(ADir, BDir).GetSafeNormal();
	float Rads = FMath::Acos(FVector::DotProduct(ADir, BDir));
	FQuat DeltaRot(AxisOfRotation, Rads);

	FQuat C = DeltaRot * AQuat;

	return FTransform(C);
}

FTransform UMWEUtil::FindNewRotation2(FTransform A, FVector BDir)
{
	
	
	BDir.Normalize();

	FRotator ARot = A.Rotator();
	FVector ADir = A.TransformVectorNoScale(FVector::ForwardVector);

	//if BDir is aligned with z
	if (FMath::IsNearlyEqual(FMath::Abs(FVector::DotProduct(BDir, FVector::UpVector)), 1.0f, 0.01f))
	{
		if (FMath::IsNearlyEqual(FMath::Abs(FVector::DotProduct(ADir, FVector::UpVector)), 1.0f, 0.01f))
		{
			PitchToDirection(ARot, BDir);
			YawToDirection(ARot, BDir);
		}
		else
		{
			YawToDirection(ARot, FVector::ForwardVector);
			PitchToDirection(ARot, BDir);
		}

		
	}
	else
	{
		

		//ADir is aligned with z
		if (FMath::IsNearlyEqual(FMath::Abs(FVector::DotProduct(ADir, FVector::UpVector)), 1.0f, 0.01f))
		{
			PitchToDirection(ARot, BDir);
			YawToDirection(ARot, BDir);
		}
		else
		{
			//opposite directions
			FVector BDirMod = BDir;
			float Dot = FVector::DotProduct(FVector::ForwardVector, BDir);
			if (Dot < 0.0f)
			{
				BDirMod = -BDir;
			}

			YawToDirection(ARot, BDirMod);
			PitchToDirection(ARot, BDir);
		}
	}


	return FTransform(ARot);
}

float UMWEUtil::CalculateTwist(FTransform A, FVector TwistAxis)
{
	/*TwistAxis.Normalize();
	FQuat ARot = A.GetRotation();
	FQuat OutSwing;
	FQuat OutTwist;
	ARot.ToSwingTwist(FVector::ForwardVector, OutSwing, OutTwist);

	float Angle = FMath::Acos(FVector::DotProduct(OutSwing.GetUpVector(), ARot.GetUpVector()));

	return FMath::RadiansToDegrees(Angle);*/

	FQuat ARot = A.GetRotation();
	FVector AFwd = ARot.GetForwardVector();
	FVector AUp = ARot.GetUpVector();

	FVector ProjectedUp;
	if (FMath::IsNearlyEqual(1.0f, FMath::Abs(FVector::DotProduct(FVector::UpVector, AFwd)), 0.01f))
	{
		ProjectedUp = -FVector::ForwardVector;
	}
	else
	{
		ProjectedUp = FVector::VectorPlaneProject(FVector::UpVector, AFwd).GetSafeNormal();
	}

	float AngleRads = FMath::Acos(FVector::DotProduct(ProjectedUp, AUp));
	return FMath::RadiansToDegrees(AngleRads);

	//float DistToFwd = FVector::DistSquared(AFwd.ProjectOnToNormal(FVector::ForwardVector), AFwd); 
	//float DistToRight = FVector::DistSquared(AFwd.ProjectOnToNormal(FVector::RightVector), AFwd); 
	//float DistToUp = FVector::DistSquared(AFwd.ProjectOnToNormal(FVector::UpVector), AFwd); 

	////find which one becomes our plane normal
	//FVector PlaneNormal;
	//FVector PlaneUp;
	//if (DistToFwd <= DistToRight && DistToFwd <= DistToUp)
	//{
	//	PlaneNormal = FVector::ForwardVector;
	//	PlaneUp = FVector::UpVector;
	//}
	//else if (DistToRight <= DistToFwd && DistToRight <= DistToUp)
	//{
	//	PlaneNormal = FVector::RightVector;
	//	PlaneUp = FVector::UpVector;
	//}
	//else
	//{
	//	PlaneNormal = FVector::UpVector;
	//	PlaneUp = FVector::ForwardVector;
	//}
	//
	//FVector AUp = ARot.GetUpVector();
	//FVector ProjectedUp = FVector::VectorPlaneProject(AUp, PlaneNormal).GetSafeNormal();
	//
	//if (FVector::DotProduct(AUp, PlaneUp) < 0.0f)
	//{
	//	PlaneUp = -PlaneUp;
	//}

	

}

void UMWEUtil::DrawArc(UObject * WorldContextObject, FVector A, FVector B, FVector RefOrigin, int NSegments, FColor Color, float SphereRadius, float Thickness, float LifeTime)
{
	A.Normalize();
	B.Normalize();
	FVector AxisOfRotation = FVector::CrossProduct(A, B).GetSafeNormal();
	float Rads = FMath::Acos(FVector::DotProduct(A, B));
	FQuat DeltaRot(AxisOfRotation, Rads);
	FQuat StartRot = FQuat::FindBetweenNormals(FVector::ForwardVector, A);

	FQuat EndRot = DeltaRot * StartRot;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::Assert);
	ULineBatchComponent* LineBatcher = World->LineBatcher;

	for (int i = 0; i < NSegments; i++)
	{
		float Alpha1 = (float)i / (NSegments);
		float Alpha2 = (float)(i + 1) / (NSegments);
		FQuat Rot1 = FQuat::SlerpFullPath(StartRot, EndRot, Alpha1);
		FQuat Rot2 = FQuat::SlerpFullPath(StartRot, EndRot, Alpha2);
		FVector RotDir1 = (Rot1.GetForwardVector() * SphereRadius) + RefOrigin;
		FVector RotDir2 = (Rot2.GetForwardVector() * SphereRadius) + RefOrigin;
		LineBatcher->DrawLine(RotDir1, RotDir2, FLinearColor(Color), ESceneDepthPriorityGroup::SDPG_World, Thickness, LifeTime);
	}
}


FVector UMWEUtil::Perpendicular(FVector X)
{
	//see https://math.stackexchange.com/a/413235
	//for a explanation of this implementation

	FVector Y = FVector::ZeroVector;

	//find index M, which has a non-zero value in X
	int M = -1;
	for (int i = 0; i < 3; i++)
	{
		float Xi = X[i];
		if (!FMath::IsNearlyZero(Xi, KINDA_SMALL_NUMBER))
		{
			M = i;
			break;
		}
	}

	//all components of X are zero
	if (M == -1)
	{
		return Y;
	}

	//X is non-zero vector
	int N = (M + 1) % 3;
	Y[N] = X[M];
	Y[M] = -X[N];

	checkf(FMath::IsNearlyEqual(FMath::Acos(FVector::DotProduct(X, Y)), PI * 0.5f, 0.01f),
		TEXT("The computed vector was not perpendicular. Debugging required."));

	return Y;
}

void UMWEUtil::PitchToDirection(FRotator& Rotation, FVector TargetDirection)
{
	FVector SourceDirection = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
	FVector SourceProjY = FVector(SourceDirection.X, 0.0f, SourceDirection.Z).GetSafeNormal();
	FVector TargetProjY = FVector(TargetDirection.X, 0.0f, TargetDirection.Z).GetSafeNormal();

	float PitchDegs = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(SourceProjY, TargetProjY)));

	//get the sign of the pitch
	if (FVector::DotProduct(-FVector::RightVector, FVector::CrossProduct(SourceProjY, TargetProjY)) < 0.0f)
	{
		PitchDegs = -PitchDegs;
	}

	//apply the pitch
	Rotation.Pitch += PitchDegs;
}

void UMWEUtil::YawToDirection(FRotator& Rotation, FVector TargetDirection)
{
	FVector SourceDirection = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
	FVector SourceProjZ = FVector(SourceDirection.X, SourceDirection.Y, 0.0f).GetSafeNormal();
	FVector TargetProjZ = FVector(TargetDirection.X, TargetDirection.Y, 0.0f).GetSafeNormal();

	float YawDegs = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(SourceProjZ, TargetProjZ)));

	//get the sign of the yaw
	if (FVector::DotProduct(FVector::UpVector, FVector::CrossProduct(SourceProjZ, TargetProjZ)) < 0.0f)
	{
		YawDegs = -YawDegs;
	}

	//apply yaw to ARot
	Rotation.Yaw += YawDegs;
}
