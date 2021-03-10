#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <QOpenGLShaderProgram>
#include <string>
#include <map>

/**
 * @brief Shared shader resource manager sub-class
 * Implemented as a singleton to share the same resources
 * across all instances of rendering widgets.
 */
class ShaderManager {
public:
    // Delete default copy-constructor to prevent copying of singleton
    ShaderManager(const ShaderManager&) = delete;

    static ShaderManager& get();

    QOpenGLShaderProgram& shader(const std::string& name) { return mShaders[name]; }
    // const QOpenGLShaderProgram& shader(const std::string& name) const;
    
    // Checks if shader exists, and is linked
    bool valid(const std::string& name) const;

private:
    ShaderManager();
    std::map<std::string, QOpenGLShaderProgram> mShaders;
};

#endif // SHADERMANAGER_H