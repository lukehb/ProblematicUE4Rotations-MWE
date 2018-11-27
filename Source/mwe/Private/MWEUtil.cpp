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
	FQuat AQuat = A.GetRotation();
	FVector ADir = AQuat.GetForwardVector();

	ADir.Normalize();
	BDir.Normalize();

	FVector AxisOfRotation = FVector::CrossProduct(ADir, BDir).GetSafeNormal();
	float Rads = FMath::Acos(FVector::DotProduct(ADir, BDir));
	FQuat DeltaRot(AxisOfRotation, Rads);

	FQuat C = DeltaRot * AQuat;

	FQuat OutSwing;
	FQuat OutTwist;

	C.ToSwingTwist(FVector::ForwardVector, OutSwing, OutTwist);

	//float TwistRads = OutTwist.GetAngle();
	//UE_LOG(LogTemp, Warning, TEXT("Twist %f"), TwistRads);

	return FTransform(OutSwing);
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

