#include "MyBossAttackNotify.h"
#include "MyBoss.h"

void UMyBossAttackNotify::Notify(
	USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation)
{
	if(AMyBoss *Boss = Cast<AMyBoss>(MeshComp->GetOwner()))
	{
		Boss->OnAttackNotify();
	}
	else
	{
	}
}
