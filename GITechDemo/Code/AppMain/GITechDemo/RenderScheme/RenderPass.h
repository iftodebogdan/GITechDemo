#ifndef RENDER_PASS_H_
#define RENDER_PASS_H_

#include <vector>

#define IMPLEMENT_RENDER_PASS(Class) \
	public: \
	Class (const char* const passName, RenderPass* const parentPass); \
	~ Class (); \
	protected: \
	void Update(const float fDeltaTime); \
	void Draw();

namespace GITechDemoApp
{
	class RenderPass
	{
	public:
		RenderPass(const char* const passName, RenderPass* const parentPass);
		~RenderPass();

		void AddChildPass(RenderPass* const childPass);
		const char* GetPassName() { return m_szPassName.c_str(); }

	protected:
		virtual void Update(const float fDeltaTime) {}
		virtual void Draw();

		void DrawChildren();

	private:
		// Disallow some member functions
		RenderPass();
		RenderPass(const RenderPass&);
		void operator=(const RenderPass&) {}

		std::string					m_szPassName;
		std::vector<RenderPass*>	m_arrChildList;

		friend class RenderScheme;
	};
}

#endif //RENDER_PASS_H_