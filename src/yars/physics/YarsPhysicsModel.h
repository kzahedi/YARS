#ifndef __YARS_PHYSICS_MODEL_H__
#define __YARS_PHYSICS_MODEL_H__

#include <yars/physics/PhysicsInterface.h>
#include <yars/physics/bullet/BulletPhysics.h>

#include <functional>

/** \brief Physics model for YARS simulation.
 *
 * Manages the physics engine and provides callbacks for reset/quit events.
 */
class YarsPhysicsModel
{
public:
  using ResetCallback = std::function<void()>;
  using QuitCallback = std::function<void()>;

  YarsPhysicsModel();
  ~YarsPhysicsModel();

  void initialisePhysics();
  void performOneSimulationStep();
  void performMultipleSimulationSteps(int numberOfSteps);
  void shutdown();
  void reset();

  /** \brief Set callback for physics-triggered reset. */
  void setResetCallback(ResetCallback callback) { _resetCallback = std::move(callback); }

  /** \brief Set callback for physics-triggered quit. */
  void setQuitCallback(QuitCallback callback) { _quitCallback = std::move(callback); }

private:
  PhysicsInterface *_physics;
  ResetCallback _resetCallback;
  QuitCallback _quitCallback;
};

#endif // __YARS_PHYSICS_MODEL_H__
