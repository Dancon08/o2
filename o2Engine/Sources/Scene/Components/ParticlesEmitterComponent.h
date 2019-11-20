#pragma once

#include "Render/ParticlesEmitter.h"
#include "Scene/DrawableComponent.h"
#include "Utils/Editor/Attributes/EditorPropertyAttribute.h"

namespace o2
{
	class ParticlesEmitterComponent: public DrawableComponent
	{
		SERIALIZABLE(ParticlesEmitterComponent);

	public:
		ParticlesEmitterComponent();
		ParticlesEmitterComponent(const ParticlesEmitterComponent& other);
		~ParticlesEmitterComponent();

		ParticlesEmitterComponent& operator=(const ParticlesEmitterComponent& other);

		// Draw particle system
		void Draw() override;

		// Updates component
		void Update(float dt) override;

		// Returns name of component
		String GetName() const override;

		// Returns category of component
		String GetCategory() const override;

		// Returns name of component icon
		String GetIcon() const override;

	protected:
		ParticlesEmitter mEmitter; // @SERIALIZABLE @EDITOR_PROPERTY

	protected:
		// It is called when actor's transform was changed
		void OnTransformUpdated();
	};
}

CLASS_BASES_META(o2::ParticlesEmitterComponent)
{
	BASE_CLASS(o2::DrawableComponent);
}
END_META;
CLASS_FIELDS_META(o2::ParticlesEmitterComponent)
{
	PROTECTED_FIELD(mEmitter).EDITOR_PROPERTY_ATTRIBUTE().SERIALIZABLE_ATTRIBUTE();
}
END_META;
CLASS_METHODS_META(o2::ParticlesEmitterComponent)
{

	PUBLIC_FUNCTION(void, Draw);
	PUBLIC_FUNCTION(void, Update, float);
	PUBLIC_FUNCTION(String, GetName);
	PUBLIC_FUNCTION(String, GetCategory);
	PUBLIC_FUNCTION(String, GetIcon);
	PROTECTED_FUNCTION(void, OnTransformUpdated);
}
END_META;
