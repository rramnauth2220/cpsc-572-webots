"""e-puck_trace_walls controller."""
# course: cpsc 572
# date: 10/26/2019
# author: Rebecca Ramnauth
# description: robot traces the room's perimeter

from controller import Robot, Motor, DistanceSensor

TIME_STEP = 64
MAX_SPEED = 6.28
LIGHT_THRESHOLD = 500
NOSE_SENSITIVITY = 0.25

robot = Robot()

# initialize devices
ps = []
psNames = [
    'ps0', 'ps1', 'ps2', 'ps3',
    'ps4', 'ps5', 'ps6', 'ps7'
]

for i in range(8): # get sensor data
    ps.append(robot.getDistanceSensor(psNames[i]))
    ps[i].enable(TIME_STEP)
    
leftMotor = robot.getMotor('left wheel motor')
rightMotor = robot.getMotor('right wheel motor')

leftMotor.setPosition(float('inf'))
rightMotor.setPosition(float('inf'))

leftMotor.setVelocity(0.0)
rightMotor.setVelocity(0.0)

while robot.step(TIME_STEP) != -1:
    # read sensor data
    psValues = []
    for i in range(8):
        psValues.append(ps[i].getValue())
    
    # detect relative walls
    top_wall = psValues[0] > (LIGHT_THRESHOLD * NOSE_SENSITIVITY) or psValues[7] > (LIGHT_THRESHOLD * NOSE_SENSITIVITY)
    side_wall = psValues[1] > LIGHT_THRESHOLD or psValues[2] > LIGHT_THRESHOLD
    
    # adjust motor speeds according to walls
    leftSpeed = MAX_SPEED
    rightSpeed = MAX_SPEED
    if top_wall:
        print('top wall detected')
        leftSpeed = 0
    elif side_wall:
        print('side wall detected')
        leftSpeed = MAX_SPEED
    
    # write to actuators
    leftMotor.setVelocity(leftSpeed)
    rightMotor.setVelocity(rightSpeed)