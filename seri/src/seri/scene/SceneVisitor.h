#pragma once

#include <memory>

namespace seri
{
	struct IScene;

	struct ISceneVisitor
	{
		virtual ~ISceneVisitor() = default;

		virtual void Visit(std::shared_ptr<IScene>& scene) = 0;

	};

	template<typename F>
	struct SceneVisitorWrapper : ISceneVisitor
	{
		SceneVisitorWrapper(F f) : _f(std::move(f)) {}

		~SceneVisitorWrapper() override = default;

		void Visit(std::shared_ptr<IScene>& scene) override
		{
			_f(scene);
		}

	private:
		F _f;

	};

	template<typename F>
	inline std::shared_ptr<ISceneVisitor> MakeSceneVisitor(F f)
	{
		return std::make_shared<SceneVisitorWrapper<F>>(std::move(f));
	}
}
