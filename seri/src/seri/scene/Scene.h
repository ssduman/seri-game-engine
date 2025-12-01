#pragma once

#include "seri/scene/SceneVisitor.h"
#include "seri/core/Object.h"
#include "seri/logging/Logger.h"

#include <memory>
#include <string>
#include <vector>
#include <algorithm>

namespace seri
{
	struct IScene
	{
		IScene(std::string name) : _id(GenerateId()), _name(std::move(name)) {}

		virtual ~IScene() = default;

		virtual void Draw() = 0;

		virtual void Add(std::shared_ptr<IScene> child) = 0;

		virtual void Remove(std::shared_ptr<IScene> child) = 0;

		virtual void Visit(std::shared_ptr<ISceneVisitor>& visitor) = 0;

		int GetId()
		{
			return _id;
		}

		bool IsLeaf()
		{
			return _children.empty();
		}

		const std::string& GetName()
		{
			return _name;
		}

		void SetName(std::string name)
		{
			_name = std::move(name);
		}

		const std::vector<std::shared_ptr<IScene>>& GetChildren()
		{
			return _children;
		}

		const std::weak_ptr<IScene>& GetParent()
		{
			return _parent;
		}

		void SetParent(std::weak_ptr<IScene> parent)
		{
			_parent = std::move(parent);
		}

		const std::shared_ptr<Object>& GetUnderlyingObject()
		{
			return _object;
		}

		void SetUnderlyingObject(std::shared_ptr<Object> object)
		{
			_object = std::move(object);
		}

		static IScene* Get(std::shared_ptr<IScene> scene, int id)
		{
			if (!scene)
			{
				return nullptr;
			}

			if (scene->GetId() == id)
			{
				return scene.get();
			}

			if (scene->IsLeaf())
			{
				return nullptr;
			}

			for (auto& s : scene->GetChildren())
			{
				auto found = IScene::Get(s, id);
				if (found)
				{
					return found;
				}
			}

			return nullptr;
		}

	protected:
		int _id{ 0 };
		std::string _name;

		std::weak_ptr<IScene> _parent;
		std::shared_ptr<Object> _object;
		std::vector<std::shared_ptr<IScene>> _children;

	private:
		int GenerateId()
		{
			return ++IScene::_id_s;
		}

		inline static int _id_s{ 0 };

	};

	struct SceneComponent : IScene, std::enable_shared_from_this<IScene>
	{
		SceneComponent(std::string name) : IScene(std::move(name)) {}

		~SceneComponent() override = default;

		void Draw() override
		{
			if (_object)
			{
				_object->Display();
			}

			for (auto& child : _children)
			{
				child->Draw();
			}
		}

		void Visit(std::shared_ptr<ISceneVisitor>& visitor) override
		{
			auto self = shared_from_this();
			visitor->Visit(self);

			for (auto& child : _children)
			{
				child->Visit(visitor);
			}
		}

		void Add(std::shared_ptr<IScene> child) override
		{
			auto self = shared_from_this();
			child->SetParent(self);
			_children.emplace_back(std::move(child));
		}

		void Remove(std::shared_ptr<IScene> child) override
		{
			_children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());
		}

	};
}
