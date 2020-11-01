#include "Application.h"

Camera3D::Camera3D(Application* app, bool start_enabled) : Module(app, start_enabled) {

	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);
	Position = vec3(0.0f, 0.0f, 5.0f);
	Reference = vec3(0.0f, 0.0f, 0.0f);

}


Camera3D::~Camera3D() {}


bool Camera3D::Start() {

	bool ret = true;
	return ret;

}


bool Camera3D::CleanUp() { return true; }


update_status Camera3D::Update(float dt)
{
	speed = 5.0f * dt;
	int mouseWheel = App->input->GetMouseZ();

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) { speed = 10.0f * dt; }
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) { MoveCamera(); }

	else if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {

		GameObject* focus = App->editorScene->GetFocus();

		if (focus != nullptr) {

			Transform* transform = (Transform*)focus->FindComponent(COMPONENT_TYPE::TRANSFORM);
			Reference = vec3(transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z);
			MoveCamera();

		}

	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {

		GameObject* object;
		object = App->editorScene->GetFocus();

		if (object != nullptr) {

			float3 vector = object->worldTransform.TranslatePart();
			Position.x = vector.x + 10;	// TODO: instead of arbitrary number, when I have boundingboxes, pick their length and vector + length 3 / 2 or so
			Position.y = vector.y + 5;
			Position.z = vector.z;
			LookAt(vec3(vector.x, vector.y, vector.z));

		}

		else { LOG("There is no gameObject selected.\n"); }
	}

	if (mouseWheel != 0 && App->editorScene->sceneWindowFocus) { Position -= Z * mouseWheel * speed; }

	CalculateViewMatrix();

	return update_status::UPDATE_CONTINUE;
}


void Camera3D::Look(const vec3& Position, const vec3& Reference, bool RotateAroundReference) {

	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference) {

		this->Reference = this->Position;
		this->Position += Z * 0.05f;

	}

	CalculateViewMatrix();

}


void Camera3D::LookAt(const vec3& Spot) {

	Reference = Spot;
	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);
	CalculateViewMatrix();

}


void Camera3D::Move(const vec3& Movement) {

	Position += Movement;
	Reference += Movement;
	CalculateViewMatrix();

}


float* Camera3D::GetViewMatrix() { return &ViewMatrix; }


void Camera3D::CalculateViewMatrix() {

	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);

}


void Camera3D::MoveCamera() {

	vec3 newPos(0, 0, 0);

	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();
	float Sensitivity = 0.25f;

	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) newPos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) newPos.y -= speed;
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	Position += newPos;
	Reference += newPos;
	Position -= Reference;

	if (dx != 0) {

		float DeltaX = (float)dx * Sensitivity;
		X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));

	}

	if (dy != 0) {

		float DeltaY = (float)dy * Sensitivity;
		Y = rotate(Y, DeltaY, X);
		Z = rotate(Z, DeltaY, X);

		if (Y.y < 0.0f) {

			Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
			Y = cross(Z, X);

		}
	}

	Position = Reference + Z * length(Position);

}
