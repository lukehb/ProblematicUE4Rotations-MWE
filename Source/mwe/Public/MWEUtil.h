#pragma once

#include "CoreMinimal.h"
#include "ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MWEUtil.generated.h"

/**
 * Util of functions needed to demonstrate rotations problem
 */
UCLASS()
class MWE_API UMWEUtil : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "MWE Utility")
	static FTransform FindNewRotation(FTransform A, FVector BDir);

	UFUNCTION(BlueprintCallable, Category = "MWE Utility")
	static FTransform FindNewRotation2(FTransform A, FVector BDir);

	UFUNCTION(BlueprintCallable, Category = "MWE Utility")
	static float CalculateTwist(FTransform A, FVector TwistAxis);

	UFUNCTION(BlueprintCallable, Category = "MWE Utility")
	static void DrawArc(UObject* WorldContextObject, FVector A, FVector B, FVector RefOrigin, int NSegments, FColor Color, float SphereRadius, float Thickness, float LifeTime);

	static FVector Perpendicular(FVector X);

};