#pragma once

#include "Actor.h"

#include "../stdafx.h"
#include "../math/matrix.h"

#include "renderer.h"

namespace Grafkit {

    class Camera;
    // 

    /** Basic camera class */
    __declspec(align(16))
        class Camera : public Entity3D, public AlignedNew<Camera>
    {
    public:
        enum camera_mode { CAMERA_LH, CAMERA_RH };

        Camera();
        explicit Camera(camera_mode mode);
        ~Camera();

        void SetAspect(float aspect) { this->m_aspect = aspect; }
        /// @todo GetAspect()

        /// Horizontal FOV in radian
        void SetFOV(float fov) { this->m_hFov = fov; }
        float GetFOV() const { return this->m_hFov; }

        void SetClippingPlanes(float znear, float zfar) { this->m_znear = znear, this->m_zfar = zfar; }
        /// @todo void GetClippingPlanes(float &znear, float &zfar);

        void SetScreenMetrics(float width, float height) { this->m_screenWidth = width, this->m_screenHeight = height; }
        /// @todo void GetScreenMetrics(float width, float height) { this->m_screenWidth = width, this->m_screenHeight = height; }

        /** Generate matrices */
        void Calculate(Renderer& renderer, ActorRef parent = nullptr) override;

        //skip render, nothing to do with it. 
        void Render(Renderer& deviceContext, SceneGraph* const & scene) override {}
        void Build(Renderer& deviceContext, SceneGraph* const & scene) override {}

        // TODO: rename them
        Matrix& GetViewMatrix() { return m_viewMatrix; }
        Matrix& GetPerspectiveMatrix() { return m_perspectiveMatrix; }
        Matrix& GetOrthoMatrix() { return m_orthoMatrix; }

        Matrix& GetWorldMatrix() { return m_worldMatrix; }

        virtual Matrix& ProjectionMatrix() {
            switch (m_type) {
            case ORTHOGRAPHIC:
                return m_orthoMatrix;
                //case PERSPECTIVE:
            default:
                return m_perspectiveMatrix;
            }
        }

        enum camera_mode GetMode() const {
            return m_mode;
        }

        enum camera_type { PERSPECTIVE, ORTHOGRAPHIC };

        virtual void setType(enum camera_type t) {
            m_type = t;
        }

        virtual enum camera_type getType() {
            return m_type;
        }

    private:
        float m_znear, m_zfar;
        float m_aspect, m_screenWidth, m_screenHeight;
        float m_hFov;

        enum camera_type m_type; // hack: van, ahol kezzel kell a kamerat atvaltnai perpective/ortho kozott
        enum camera_mode m_mode;

    protected:
        Matrix m_worldMatrix;
        Matrix m_viewMatrix;
        Matrix m_perspectiveMatrix;
        Matrix m_orthoMatrix;

        PERSISTENT_DECL(Grafkit::Camera, 1);

    public:
        void Serialize(Archive& ar) override;
    };
}
