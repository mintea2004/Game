#include"Camera.h"
#include <algorithm>

void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	/*DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View); 
	*/

	//shake
	DirectX::XMFLOAT3 shakenEye = eye;
	shakenEye.x += shakeOffset.x;
	shakenEye.y += shakeOffset.y;
	shakenEye.z += shakeOffset.z;

	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&shakenEye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);


	this->right.x = view._11;
	this->right.y = view._21;
	this->right.z = view._31;

	this->up.x = view._12;
	this->up.y = view._22;
	this->up.z = view._32;

	this->front.x = view._13;
	this->front.y = view._23;
	this->front.z = view._33;

	//this->eye = eye;
	this->eye = shakenEye;
	this->focus = focus;
}

void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);

	DirectX::XMStoreFloat4x4(&projection, Projection);
}

void Camera::AddShake(float strength)
{
	shakeTime = 0.15f;
	shakeStrength = std::max(shakeStrength, strength);
}

void Camera::UpdateShake(float elapsedTime)
{
	if (shakeTime <= 0.0f)
	{
		shakeOffset = { 0, 0, 0 };
		return;
	}

	shakeTime -= elapsedTime;

	float decay = shakeTime / 0.15f; // smooth fade

	// random shake
	shakeOffset.x = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * shakeStrength * decay;
	shakeOffset.y = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * shakeStrength * decay;
	shakeOffset.z = 0.0f; // usually no Z shake
}
