#pragma once

struct IEngineBackend {

    virtual ~IEngineBackend() = default;

    virtual void draw() = 0;

    virtual void release() = 0;

};