#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle3D::PhysVehicle3D(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody3D(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle3D::~PhysVehicle3D()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Render()
{
	Cylinder wheel;

	wheel.color = Blue;

	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		wheel.radius = info.wheels[0].radius;
		wheel.height = info.wheels[0].width;

		vehicle->updateWheelTransform(i);
		vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);
		wheel.color = Black;

		wheel.Render();
	}

	Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	chassis.transform.M[12] += offset.getX();
	chassis.transform.M[13] += offset.getY();
	chassis.transform.M[14] += offset.getZ();
	chassis.color = Red;

	Cube roof(info.roof_size.x, info.roof_size.y, info.roof_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&roof.transform);
	btVector3 offsetroof(info.roof_offset.x, info.roof_offset.y, info.roof_offset.z);
	offsetroof = offsetroof.rotate(q.getAxis(), q.getAngle());
	roof.transform.M[12] += offsetroof.getX();
	roof.transform.M[13] += offsetroof.getY();
	roof.transform.M[14] += offsetroof.getZ();
	roof.color = Red;

	Cube leftstick(info.leftstick_size.x, info.leftstick_size.y, info.leftstick_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&leftstick.transform);
	btVector3 offsetleftstick(info.leftstick_offset.x, info.leftstick_offset.y, info.leftstick_offset.z);
	offsetleftstick = offsetleftstick.rotate(q.getAxis(), q.getAngle());
	leftstick.transform.M[12] += offsetleftstick.getX();
	leftstick.transform.M[13] += offsetleftstick.getY();
	leftstick.transform.M[14] += offsetleftstick.getZ();
	leftstick.color = Red;

	Cube rightstick(info.rightstick_size.x, info.rightstick_size.y, info.rightstick_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&rightstick.transform);
	btVector3 offsetrightstick(info.rightstick_offset.x, info.rightstick_offset.y, info.rightstick_offset.z);
	offsetrightstick = offsetrightstick.rotate(q.getAxis(), q.getAngle());
	rightstick.transform.M[12] += offsetrightstick.getX();
	rightstick.transform.M[13] += offsetrightstick.getY();
	rightstick.transform.M[14] += offsetrightstick.getZ();
	rightstick.color = Red;

	Cube aileron(info.aileron_size.x, info.aileron_size.y, info.aileron_size.z);
	vehicle->getChassisWorldTransform().getOpenGLMatrix(&aileron.transform);
	btVector3 offsetaileron(info.aileron_offset.x, info.aileron_offset.y, info.aileron_offset.z);
	offsetaileron = offsetaileron.rotate(q.getAxis(), q.getAngle());
	aileron.transform.M[12] += offsetaileron.getX();
	aileron.transform.M[13] += offsetaileron.getY();
	aileron.transform.M[14] += offsetaileron.getZ();
	aileron.color = Red;

	chassis.Render();
	roof.Render();
	leftstick.Render();
	rightstick.Render();
	aileron.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::ApplyEngineForce(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Brake(float force)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].brake == true)
		{
			vehicle->setBrake(force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle3D::Turn(float degrees)
{
	for(int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if(info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue(degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle3D::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}