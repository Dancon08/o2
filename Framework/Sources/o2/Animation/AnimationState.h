#pragma once

#include "o2/Animation/AnimationMask.h"
#include "o2/Animation/AnimationPlayer.h"
#include "o2/Assets/Types/AnimationAsset.h"
#include "o2/Utils/Editor/Attributes/InvokeOnChangeAttribute.h"
#include "o2/Utils/Serialization/Serializable.h"
#include "o2/Utils/Types/Containers/Vector.h"

namespace o2
{
	class AnimationComponent;

	// ---------------
	// Animation state
	// ---------------
	class AnimationState: public ISerializable
	{
	public:
		String        name; // State name @SERIALIZABLE
		AnimationMask mask; // Animation mask @SERIALIZABLE

		float blend = 1.0f; // State blending coefficient in 0..1 Used for blending

		AnimationPlayer player; // Animation player

	public:
		// Default constructor
		AnimationState() = default;

		// Constructor with name
		AnimationState(const String& name);

		// Sets state weight
		void SetWeight(float weight);

		// Returns state weight
		float GetWeight() const;

		// Sets animation
		void SetAnimation(const AnimationAssetRef& animationAsset);

		// Returns animation
		const AnimationAssetRef& GetAnimation() const;

		SERIALIZABLE(AnimationState);

	protected:
		AnimationComponent* mOwner = nullptr; // Animation state owner component
		AnimationAssetRef   mAnimation;       // Animation @SERIALIZABLE @EDITOR_PROPERTY @INVOKE_ON_CHANGE(OnAnimationChanged)
		float               mWeight = 1.0f;   // State weight @SERIALIZABLE @EDITOR_PROPERTY

	protected:
		// It is called when animation changed from editor
		void OnAnimationChanged();

		// It is called when player has added new track
		void OnTrackPlayerAdded(IAnimationTrack::IPlayer* trackPlayer);

		// It is called when player is removing track
		void OnTrackPlayerRemove(IAnimationTrack::IPlayer* trackPlayer);

		friend class AnimationComponent;
		friend class AnimationClip;

		template<typename _type>
		friend class AnimationTrack;
	};
}

CLASS_BASES_META(o2::AnimationState)
{
	BASE_CLASS(o2::ISerializable);
}
END_META;
CLASS_FIELDS_META(o2::AnimationState)
{
	PUBLIC_FIELD(name).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(mask).SERIALIZABLE_ATTRIBUTE();
	PUBLIC_FIELD(blend).DEFAULT_VALUE(1.0f);
	PUBLIC_FIELD(player);
	PROTECTED_FIELD(mOwner).DEFAULT_VALUE(nullptr);
	PROTECTED_FIELD(mAnimation).EDITOR_PROPERTY_ATTRIBUTE().INVOKE_ON_CHANGE_ATTRIBUTE(OnAnimationChanged).SERIALIZABLE_ATTRIBUTE();
	PROTECTED_FIELD(mWeight).DEFAULT_VALUE(1.0f).EDITOR_PROPERTY_ATTRIBUTE().SERIALIZABLE_ATTRIBUTE();
}
END_META;
CLASS_METHODS_META(o2::AnimationState)
{

	PUBLIC_FUNCTION(void, SetWeight, float);
	PUBLIC_FUNCTION(float, GetWeight);
	PUBLIC_FUNCTION(void, SetAnimation, const AnimationAssetRef&);
	PUBLIC_FUNCTION(const AnimationAssetRef&, GetAnimation);
	PROTECTED_FUNCTION(void, OnAnimationChanged);
	PROTECTED_FUNCTION(void, OnTrackPlayerAdded, IAnimationTrack::IPlayer*);
	PROTECTED_FUNCTION(void, OnTrackPlayerRemove, IAnimationTrack::IPlayer*);
}
END_META;
