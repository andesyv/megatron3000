#ifndef NIFTIIMAGE_H
#define NIFTIIMAGE_H

#include <nifti1_io.h>
#include <string>
#include <optional>
#include <memory>

// Nifti Image RAII wrapper
class NiftiImage
{
private:
    struct ImageDeleter
    {
        void operator()(nifti_image *ptr)
        {
            if (ptr != nullptr)
                nifti_image_free(ptr);
        }
    };

    std::unique_ptr<nifti_image, ImageDeleter> m_image{};
    NiftiImage() = default;

public:
    static std::optional<NiftiImage> make(std::string file)
    {
        if (is_nifti_file(file.c_str()) < 0)
            return std::nullopt;

        NiftiImage image;
        image.m_image.reset(nifti_image_read(file.c_str(), true));
        return image;
    }

    nifti_image const &get() const
    {
        return *m_image;
    }

    const auto &operator*() const
    {
        return get();
    }
};

#endif // NIFTIIMAGE_H