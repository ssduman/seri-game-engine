# Seri Game Engine C++ Reference

This is the reference for the engine library in `seri/seri`, generated from the sources with Doxygen.
Use the tree on the left, or the search box, to browse namespaces, classes and files.

[Website](../index.html) | [Lua scripting reference](../lua.html) | [Source on GitHub](https://github.com/ssduman/seri-game-engine)

## Where to start

- seri::Application runs the main loop and owns the layers, seri::CoreLayer updates the systems each frame
- seri::Entity wraps an EnTT entity, components live in the seri::component namespace
- seri::scene::SceneManager loads, saves and plays scenes, seri::asset::AssetManager tracks project assets
- seri::system holds the per-frame systems such as seri::system::TransformSystem, seri::system::PhysicsSystem and seri::system::ScriptSystem
- seri::script::LuaBindings defines everything Lua scripts can use
- seri::project::ProjectManager opens and creates projects

## Frame order

Fixed steps (scripts, then physics), then script update and late update, then transforms, cameras and rendering.
