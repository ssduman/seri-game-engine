#include <GL/glew.h>

#include "glm/glm.hpp"

#include <fstream>
#include <string>
#include <sstream>

class Shader {
private:
	unsigned int ID = 0;
	std::string loadshader(const std::string &path);

public:
	Shader(const std::string &vs_path, const std::string &fs_path);
	~Shader();
	void bind();
	void unbind();
	void setUmat4f(const std::string &name, const glm::mat4 &value) const;
};
