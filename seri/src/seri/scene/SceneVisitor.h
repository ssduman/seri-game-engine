#pragma once

#include <memory>

struct IScene;

struct ISceneVisitor
{
	virtual ~ISceneVisitor() = default;

	virtual void visit(std::shared_ptr<IScene>& scene) = 0;

};

template<typename F>
struct SceneVisitorWrapper : ISceneVisitor
{
	SceneVisitorWrapper(F f) : _f(std::move(f)) {}

	~SceneVisitorWrapper() override = default;

	void visit(std::shared_ptr<IScene>& scene) override
	{
		_f(scene);
	}

private:
	F _f;

};

template<typename F>
inline std::shared_ptr<ISceneVisitor> makeSceneVisitor(F f)
{
	return std::make_shared<SceneVisitorWrapper<F>>(std::move(f));
}
