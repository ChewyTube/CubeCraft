#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UPWARD,
    DOWNWARD
};

//�������ʼֵ
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 8.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:
    // ���������
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // ŷ����
    float Yaw;
    float Pitch;
    // �����ѡ��
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    // �������캯��
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // constructor with scalar values
    // �������캯��
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    // ʹ��ŷ���Ǻ�LookAt���󷵻�view����
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    // ���ܼ������벢��Ӧ
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        glm::vec3 plarerDirection;
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
        {
            plarerDirection = glm::vec3(Front.x, 0.0f, Front.z);
            Position += glm::normalize(plarerDirection) * velocity;
        }
        if (direction == BACKWARD)
        {
            plarerDirection = glm::vec3(Front.x, 0.0f, Front.z);
            Position -= glm::normalize(plarerDirection) * velocity;
        }
        if (direction == LEFT)
        {
            plarerDirection = glm::vec3(Right.x, 0.0f, Right.z);
            Position -= glm::normalize(plarerDirection) * velocity;
        }
        if (direction == RIGHT)
        {
            plarerDirection = glm::vec3(Right.x, 0.0f, Right.z);
            Position += glm::normalize(plarerDirection) * velocity;
        }
        if (direction == UPWARD)
            Position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
        if (direction == DOWNWARD)
            Position -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    // ����������벢��Ӧ  �ı��ӽǷ���
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        // ���Ƹ����� ������Ļ��ת
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        // ʹ��ŷ���Ǹ�������
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    // �����ֵ������� �ѷϳ�
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    // ʹ��ŷ���Ǹ�������
    void updateCameraVectors()
    {
        // calculate the new Front vector
        // ����������
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        // ������������������
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};