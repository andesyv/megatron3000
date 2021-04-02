#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <QOpenGLShaderProgram>
#include <string>
#include <map>
#include <QObject>
#include <filesystem>

/**
 * @brief Helper wrapper class for shaders that remembers their shaderfiles so they can be reloaded 
 * 
 */
class Shader : public QOpenGLShaderProgram {
public:
    Shader(QObject *parent = nullptr) : QOpenGLShaderProgram{parent} {}

    bool addSource(QOpenGLShader::ShaderType type, const std::filesystem::path &filePath);
    bool addSourceRelative(QOpenGLShader::ShaderType type, const std::string &filePath);
    bool addSource(QOpenGLShader::ShaderType type, const QString &fileName);

    bool reload();

private:
    std::map<QOpenGLShader::ShaderType, QString> mPath;
};

/**
 * @brief Shared shader resource manager sub-class
 * Implemented as a singleton to share the same resources
 * across all instances of rendering widgets.
 */
class ShaderManager : public QObject {
public:
    // Delete default copy-constructor to prevent copying of singleton
    ShaderManager(const ShaderManager&) = delete;

    static ShaderManager& get();

    Shader& shader(const std::string& name) { return mShaders[name]; }
    // const QOpenGLShaderProgram& shader(const std::string& name) const;
    
    // Checks if shader exists, and is linked
    bool valid(const std::string& name) const;

private:
    ShaderManager();
    std::map<std::string, Shader> mShaders;

public slots:
    bool reloadShaders();
};

#endif // SHADERMANAGER_H