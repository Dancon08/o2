#pragma once

#include "Render/Camera.h"
#include "Render/TextureRef.h"
#include "Render/Windows/RenderBase.h"
#include "Utils/Math/Vertex2.h"
#include "Utils/Singleton.h"

// Render access macros
#define o2Render o2::Render::Instance()

namespace o2
{
	class Mesh;

	// ------------------
	// 2D Graphics render
	// ------------------
	class Render: public RenderBase, public Singleton<Render>
	{
	public:
		Getter<Vec2I>        Resolution;             // Screen resolution getter
		Property<Camera>     CurrentCamera;          // Current camera property
		Property<RectI>      ScissorRect;            // Scissor rect property
		Property<TextureRef> RenderTexture;          // Render target texture property
		Getter<bool>         RenderTextureAvailable; // Render textures available getter
		Getter<Vec2I>        MaxTextureSize;         // Maximal texture size getter

		// Default constructor
		Render();

		// Destructor
		~Render();

		// Beginning rendering
		void Begin();

		// Finishing rendering
		void End();

		// Clearing current frame buffer with color
		void Clear(const Color4& color = Color4::Blue());

		// Returns resolution of rendering frame
		Vec2I GetResolution() const;

		// Binding camera. NULL - standard camera
		void SetCamera(const Camera& camera);

		// Returns current camera
		Camera GetCamera() const;

		// Draw single line with color
		void DrawLine(const Vec2F& a, const Vec2F& b, const Color4& color = Color4::White());

		// Draw rect frame with color
		void DrawRectFrame(const Vec2F& minp, const Vec2F& maxp, const Color4& color = Color4::White());

		// Draw rect frame with color
		void DrawRectFrame(const RectF& rect, const Color4& color = Color4::White());

		void DrawBasis(const Basis& basis, const Color4& xcolor = Color4::Red(), const Color4& ycolor = Color4::Blue(),
					   const Color4& color = Color4::White());

		// Draw cross with color
		void DrawCross(const Vec2F& pos, float size = 5, const Color4& color = Color4::White());

		// Draw circle with color
		void DrawCircle(const Vec2F& pos, float radius = 5, const Color4& color = Color4::White());

		// Beginning render to stencil buffer
		void BeginRenderToStencilBuffer();

		// Finishing rendering in stencil buffer
		void EndRenderToStencilBuffer();

		// Enabling stencil test
		void EnableStencilTest();

		// Disabling stencil test
		void DisableStencilTest();

		// Returns true, if stencil test enabled
		bool IsStencilTestEnabled() const;

		// Clearing stencil buffer
		void ClearStencil();

		// Sets scissor rect
		void SetupScissorRect(const RectI& rect);

		// Returns scissor rect
		RectI GetScissorRect() const;

		// Enabling scissor test
		void EnableScissorTest();

		// Disabling scissor test
		void DisableScissorTest();

		// Returns true, if scissor test enabled
		bool IsScissorTestEnabled() const;

		// Drawing mesh
		bool DrawMesh(Ptr<Mesh> mesh);

		// Drawing lines
		bool DrawLines(Vertex2* verticies, int count);

		// Binding render target
		void SetRenderTexture(TextureRef renderTarget);

		// Unbinding render target
		void UnbindRenderTexture();

		// Returns current render target. Returns NULL if no render target
		TextureRef GetRenderTexture() const;

		// Returns true, if render target is can be used with current device
		bool IsRenderTextureAvailable() const;

		// Returns maximum texture size
		Vec2I GetMaxTextureSize() const;

	protected:
		typedef Vector<Ptr<Texture>> TexturesVec;

		Ptr<LogStream> mLog;                    // Render log stream

		TexturesVec    mTextures;               // Loaded textures

		Camera         mCamera;                 // Camera transformation
		Vec2I          mResolution;             // Current back buffer size

		bool           mRenderTargetsAvailable; // True, if render targets is available
		Vec2I          mMaxTextureSize;         // Max texture size

		bool           mStencilDrawing;         // True, if drawing in stencil buffer
		bool           mStencilTest;            // True, if drawing with stencil test

		RectI          mScissorRect;            // Scissor rect, in screen space
		bool           mScissorTest;            // True, if scissor test enabled

		TextureRef     mCurrentRenderTarget;    // Current render target. NULL if rendering in back buffer

		bool           mReady;                  // True, if render system initialized

	protected:
		// Don't copy
		Render(const Render& other);

		// Don't copy
		Render& operator=(const Render& other);

		// Send buffers to draw
		void DrawPrimitives();

		// Sets orthographic view matrix by view size
		void SetupViewMatrix(const Vec2I& viewSize);

		// Updates render transformations for camera
		void UpdateCameraTransforms();

		// Checks render compatibles
		void CheckCompatibles();

		// Check textures for unloading
		void CheckTexturesUnloading();

		// initializes properties
		void InitializeProperties();

		friend class Texture;
		friend class TextureRef;
		friend class BaseApplication;
	};
}