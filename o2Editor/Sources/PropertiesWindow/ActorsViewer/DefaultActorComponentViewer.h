#pragma once

#include "Core/Properties/FieldPropertiesInfo.h"
#include "Core/Properties/IPropertyField.h"
#include "PropertiesWindow/ActorsViewer/IActorComponentViewer.h"

namespace Editor
{
	// ------------------------------
	// Default actor component viewer
	// ------------------------------
	class DefaultActorComponentViewer: public IActorComponentViewer
	{
	public:
		// Default constructor. Initializes data widget
		DefaultActorComponentViewer();

		// Virtual destructor
		~DefaultActorComponentViewer();

		// Sets target actors
		void SetTargetComponents(const Vector<Component*>& components) override;

		// Returns viewing component type 
		const Type* GetComponentType() const override;

		// Updates all component values
		void Refresh() override;

		// Rebuilds properties layout
		void Rebuild() override;

		// Specialize viewing component type. Creates all using properties
		void SpecializeComponentType(const Type* type);

		IOBJECT(DefaultActorComponentViewer);

	protected:
		FieldPropertiesInfo mFieldProperties;         // Field properties information
		const Type*         mComponentType = nullptr; // Target component type

	protected:
		// It is called when some property changed, marks Actor as changed and calls default Undo create callback
		void OnPropertyChanged(const String& path, const Vector<DataNode>& before, const Vector<DataNode>& after);
	};
}

CLASS_BASES_META(Editor::DefaultActorComponentViewer)
{
	BASE_CLASS(Editor::IActorComponentViewer);
}
END_META;
CLASS_FIELDS_META(Editor::DefaultActorComponentViewer)
{
	PROTECTED_FIELD(mFieldProperties);
	PROTECTED_FIELD(mComponentType);
}
END_META;
CLASS_METHODS_META(Editor::DefaultActorComponentViewer)
{

	PUBLIC_FUNCTION(void, SetTargetComponents, const Vector<Component*>&);
	PUBLIC_FUNCTION(const Type*, GetComponentType);
	PUBLIC_FUNCTION(void, Refresh);
	PUBLIC_FUNCTION(void, Rebuild);
	PUBLIC_FUNCTION(void, SpecializeComponentType, const Type*);
	PROTECTED_FUNCTION(void, OnPropertyChanged, const String&, const Vector<DataNode>&, const Vector<DataNode>&);
}
END_META;
