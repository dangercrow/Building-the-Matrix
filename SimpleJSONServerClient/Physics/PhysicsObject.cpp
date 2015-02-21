#include "../LocationComponent.hpp"
#include "AABB.hpp"
#include "PhysicsObject.hpp"
#include "PhysicsMaths.hpp"

PhysicsObject::PhysicsObject(std::shared_ptr<LocationComponent> locationComp, const vertexVector vertices)
	: mass(1.0f), inverseMass(1.0f),
	restitution(1.0f), velocity(vec3())
{
	if (locationComp == nullptr){
		locationComp = std::make_shared<LocationComponent>();
	}
	this->location = locationComp;
	boundingBox = std::make_shared<AABB>(vertices);
}

PhysicsObject::~PhysicsObject()
{
}

const std::shared_ptr<AABB> PhysicsObject::getWorldAABB() const {
	std::shared_ptr<vertexVector> fullBox = boundingBox->getFullBox();
	glm::mat4x4 translateMatrix = glm::translate(glm::mat4x4(1.0f), location->getPosition());
	translateMatrix *= location->getRotationMatrix();
	std::shared_ptr<vertexVector> worldSpace = PhysicsMaths::translateVertexVector(translateMatrix, fullBox);
	std::shared_ptr<AABB> worldBox = std::make_shared<AABB>(*worldSpace);
	return worldBox;
}

// TODO: Refactor me
const AABB PhysicsObject::getLocalAABB() const {
	return *boundingBox;
}

float PhysicsObject::getMass() const{
	return mass;
}

float PhysicsObject::getInvMass() const{
	return inverseMass;
}

float PhysicsObject::getRest() const{
	return restitution;
}

void PhysicsObject::setX(vec3 &newPos){
	location->setPosition(newPos);
}

void PhysicsObject::setV(vec3 &newVelocity){
	velocity = vec3(newVelocity);
}

void PhysicsObject::setA(vec3 &newAcc){
	acc = vec3(newAcc);
}

const vec3 PhysicsObject::getX() const{
	return location->getPosition();
}

const vec3 PhysicsObject::getV() const{
	return velocity;
}

const vec3 PhysicsObject::getA() const{
	return acc;
}

float PhysicsObject::getLinDrag() const{
	return friction;
}

float PhysicsObject::getQuadDrag() const{
	return airRes;
}

const vec3 PhysicsObject::getOrientation() const{
	return orientation;
}

void PhysicsObject::setOrientation(vec3 & v){
	orientation = glm::normalize(v);
}

void PhysicsObject::setMass(const float mass){
	this->mass = mass;
	if (mass == 0.0f){
		inverseMass = 0.0f;
	}
	else{
		inverseMass = 1.0f / mass;
	}
}

void PhysicsObject::setRest(const float rest){
	restitution = rest;
}

void PhysicsObject::setQuadDrag(const float quadDrag){
	airRes = quadDrag;
}

void PhysicsObject::setLinDrag(const float linDrag){
	friction = linDrag;
}

void PhysicsObject::turnObject(const Quaternion rotator, const vec3(PhysicsObject::*getter) () const, void (PhysicsObject::*setter) (vec3 &)){
	vec3 dir = (*this.*getter)();
	Quaternion oldVector = Quaternion(0.0f, dir.x, dir.y, dir.z);
	Quaternion out = rotator*oldVector;
	vec3 updated = vec3(out.x, out.y, out.z);
	(*this.*setter)(updated);
}

void PhysicsObject::acceleratePlayer(){
	vec3 dir = this->getOrientation();
	float speed = glm::length(dir);
	vec3 A = this->getA();
	// TODO: Consider further mechanisms for determining power
	// TODO: Adjust arbitrary constant according to playtesting
	this->setA(A + glm::normalize(dir)*(5.0f - speed));
}


void PhysicsObject::reversePlayer(){
	vec3 dir = this->getOrientation();
	float speed = glm::length(dir);
	vec3 A = this->getA();
	// TODO: Adjust arbitrary constant according to playtesting
	this->setA(A + glm::normalize(dir)*(-2.0f - speed));
}

void PhysicsObject::turnLeft(float turnSpeed = TURN_SPEED){
	turnObject(Quaternion(turnSpeed, 0.0f, 1.0f, 0.0f), &PhysicsObject::getOrientation, &PhysicsObject::setOrientation);
	turnObject(Quaternion(turnSpeed, 0.0f, 1.0f, 0.0f), &PhysicsObject::getV, &PhysicsObject::setV);
	turnObject(Quaternion(turnSpeed, 0.0f, 1.0f, 0.0f), &PhysicsObject::getA, &PhysicsObject::setA);

}

void PhysicsObject::turnRight(float turnSpeed = TURN_SPEED){
	turnLeft(-turnSpeed);

}