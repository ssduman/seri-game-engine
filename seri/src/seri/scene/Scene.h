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
		IScene(std::string name) : _id(generateId()), _name(std::move(name)) {}

		virtual ~IScene() = default;

		virtual void draw() = 0;

		virtual void add(std::shared_ptr<IScene> child) = 0;

		virtual void remove(std::shared_ptr<IScene> child) = 0;

		virtual void visit(std::shared_ptr<ISceneVisitor>& visitor) = 0;

		int getId()
		{
			return _id;
		}

		bool isLeaf()
		{
			return _children.empty();
		}

		const std::string& getName()
		{
			return _name;
		}

		void setName(std::string name)
		{
			_name = std::move(name);
		}

		const std::vector<std::shared_ptr<IScene>>& getChildren()
		{
			return _children;
		}

		const std::weak_ptr<IScene>& getParent()
		{
			return _parent;
		}

		void setParent(std::weak_ptr<IScene> parent)
		{
			_parent = std::move(parent);
		}

		const std::shared_ptr<Object>& getObject()
		{
			return _object;
		}

		void setObject(std::shared_ptr<Object> object)
		{
			_object = std::move(object);
		}

		static IScene* get(std::shared_ptr<IScene> scene, int id)
		{
			if (!scene)
			{
				return nullptr;
			}

			if (scene->getId() == id)
			{
				return scene.get();
			}

			if (scene->isLeaf())
			{
				return nullptr;
			}

			for (auto& s : scene->getChildren())
			{
				auto found = IScene::get(s, id);
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
		int generateId()
		{
			return ++IScene::_id_s;
		}

		inline static int _id_s{ 0 };

	};

	struct SceneComponent : IScene, std::enable_shared_from_this<IScene>
	{
		SceneComponent(std::string name) : IScene(std::move(name)) {}

		~SceneComponent() override = default;

		void draw() override
		{
			if (_object)
			{
				_object->display();
			}

			for (auto& child : _children)
			{
				child->draw();
			}
		}

		void visit(std::shared_ptr<ISceneVisitor>& visitor) override
		{
			visitor->visit(shared_from_this());

			for (auto& child : _children)
			{
				child->visit(visitor);
			}
		}

		void add(std::shared_ptr<IScene> child) override
		{
			child->setParent(shared_from_this());
			_children.emplace_back(std::move(child));
		}

		void remove(std::shared_ptr<IScene> child) override
		{
			_children.erase(std::remove(_children.begin(), _children.end(), child), _children.end());
		}

	};
}
