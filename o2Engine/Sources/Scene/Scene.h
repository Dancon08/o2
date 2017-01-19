#pragma once

#include "Utils/Containers/Vector.h"
#include "Utils/Serializable.h"
#include "Utils/Singleton.h"
#include "Utils/String.h"
#include "Utils/UID.h"

// Scene graph access macros
#define o2Scene Scene::Instance()

namespace o2
{
    class ActorAsset;
    class DrawableComponent;
    class Tag;
    
    class Actor;
    typedef Vector<Actor*> ActorsVec;
    
    class SceneLayer;
    typedef Vector<SceneLayer*> SceneLayersVec;

	// ----------
	// Scene host
	// ----------
	class Scene: public Singleton<Scene>
	{
	public:
		typedef Vector<ActorAsset*> ActorsAssetsVec;
		typedef Vector<String> StringsVec;
		typedef Vector<Tag*> TagsVec;

	public:
		Function<void(Actor*)> onActorCreated;       // Actor creation event
		Function<void(Actor*)> onActorDestroying;    // Actor destroying event
		Function<void(Actor*)> onActorEnableChanged; // Actor enable changing

#if IS_EDITOR
		Function<void(ActorsVec)> onChanged;                     // Actors some change event
		Function<void(Actor*)>    onActorLockChanged;			 // Actor locking change
		Function<void(Actor*)>    onActorNameChanged;			 // Actor name changing event
		Function<void(Actor*)>    onActorChildsHierarchyChanged; // Actor childs hierarchy change event
#endif

		// Returns layer by name
		SceneLayer* GetLayer(const String& name) const;

		// Returns default layer
		SceneLayer* GetDefaultLayer() const;

		// Adds layer with name
		SceneLayer* AddLayer(const String& name);

		// Removes layer
		void RemoveLayer(SceneLayer* layer, bool removeActors = true);

		// Removes layer by name
		void RemoveLayer(const String& name, bool removeActors = true);

		// Returns layers array
		SceneLayersVec& GetLayers();

		// Returns tag with name
		Tag* GetTag(const String& name) const;

		// Adds tag with name
		Tag* AddTag(const String& name);

		// Removes tag
		void RemoveTag(Tag* tag);

		// Removes tag with name
		void RemoveTag(const String& name);

		// Returns tags array
		const TagsVec& GetTags() const;

		// Returns root actors
		const ActorsVec& GetRootActors() const;

		// Returns root actors
		ActorsVec& GetRootActors();

		// Returns all actors
		const ActorsVec& GetAllActors() const;

		// Returns all actors
		ActorsVec& GetAllActors();

		// Returns actor by id
		Actor* GetActorByID(UInt64 id) const;

		// Returns asset actor by asset id. Tries to find in cache
		Actor* GetAssetActorByID(UID id);

		// Returns actor by path (ex "some node/other/target")
		Actor* FindActor(const String& path);

		// Returns component with type in scene
		template<typename _type>
		_type* FindActorComponent();

		// Returns all components with type in scene
		template<typename _type>
		Vector<_type*> FindAllActorsComponents();

		// Removes all actors
		void Clear();

		// Clears assets cache
		void ClearCache();

		// Loads scene from file. If append is true, old actors will not be destroyed
		void Load(const String& path, bool append = false);

		// Saves scene into file
		void Save(const String& path);

		// Returns actor's index in hierarchy
		int GetActorHierarchyIdx(Actor* actor) const;

		// Reparent actors to new parent at next of prevActor;
		void ReparentActors(const ActorsVec& actors, Actor* newParent, Actor* prevActor);

#if IS_EDITOR	  
		// Calls when actor was changed
		void OnActorChanged(Actor* actor);   

		// Checks is any actors was changed and calls OnChanged() if changed
		void CheckChangedActors();

		// Returns current changed actors
		const ActorsVec& GetChangedActors() const;
#endif       

	protected:
		ActorsVec       mRootActors;    // Scene root actors		
		ActorsVec       mAllActors;     // All scene actors
		SceneLayersVec  mLayers;        // Scene layers
		TagsVec         mTags;          // Scene tags
		SceneLayer*     mDefaultLayer;  // Default scene layer
		ActorsAssetsVec mCache;         // Cached actors assets
				  
#if IS_EDITOR	  
		ActorsVec       mChangedActors; // Changed actors array
#endif

	protected:
		// Default constructor
		Scene();

		// Destructor
		~Scene();

		// Updates root actors
		void Update(float dt);

		// Draws scene drawable components
		void Draw();

		friend class Actor;
		friend class Application;
		friend class DrawableComponent;
	};
    
    // -----------
    // Scene layer
    // -----------
    class SceneLayer: public ISerializable
    {
    public:
        typedef Vector<DrawableComponent*> DrawCompsVec;
        
    public:
        String name; // Name of layer @SERIALIZABLE
        
        // Returns all actors in layer
        const ActorsVec& GetActors() const;
        
        // Returns enabled actors in layer
        const ActorsVec& GetEnabledActors() const;
        
        // Returns all drawable components of actors in layer
        const DrawCompsVec& GetDrawableComponents() const;
        
        // Returns enabled drawable components of actors in layer
        const DrawCompsVec& GetEnabledDrawableComponents() const;
        
        SERIALIZABLE(SceneLayer);
        
    protected:
        ActorsVec    mActors;           // Actors in layer
        ActorsVec    mEnabledActors;    // Enabled actors
        DrawCompsVec mDrawables;        // Drawable components in layer
        DrawCompsVec mEnabledDrawables; // Enabled drawable components in layer
        
    protected:
        // Registers drawable component
        void RegDrawableComponent(DrawableComponent* component);
        
        // Unregisters drawable component
        void UnregDrawableComponent(DrawableComponent* component);
        
        // Calls when drawable component depth was changed and sorts all drawable component
        void ComponentDepthChanged(DrawableComponent* component);
        
        // Calls when component was enabled
        void ComponentEnabled(DrawableComponent* component);
        
        // Calls when component was enabled
        void ComponentDisabled(DrawableComponent* component);
        
        friend class DrawableComponent;
        friend class Scene;
        friend class Actor;
    };

	// -------------------------
	// Layer data node converter
	// -------------------------
	class LayerDataNodeConverter: public IDataNodeTypeConverter
	{
	public:
		// Converts layer pointer to data 
		void ToData(void* object, DataNode& data);

		// Gets layer pointer from data
		void FromData(void* object, const DataNode& data);

		// Checks that type is layer's type
		bool CheckType(const Type* type) const;
	};
}

#include "Scene/Actor.h"

namespace o2
{
	template<typename _type>
	Vector<_type*> Scene::FindAllActorsComponents()
	{
		Vector<_type*> res;
		for (auto actor : mRootActors)
			res.Add(actor->GetComponentsInChildren<_type>());

		return res;
	}

	template<typename _type>
	_type* Scene::FindActorComponent()
	{
		for (auto actor : mRootActors)
		{
			_type* res = actor->GetComponentInChildren<_type>();
			if (res)
				return res;
		}

		return nullptr;
	}
};
