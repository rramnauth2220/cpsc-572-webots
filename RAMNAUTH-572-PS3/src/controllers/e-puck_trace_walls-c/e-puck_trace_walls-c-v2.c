/*
 * File:          e-puck_trace_walls-c.c
 * Date:          10/28/2019
 * Description:   robot traces the room's perimeter
 * Author:        Rebecca Ramnauth
 * Modifications: translation from original submission in Python to C
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <webots/robot.h>
#include <webots/distance_sensor.h>
#include <webots/motor.h>

/* macros */
#define TIME_STEP 64
#define MAX_SPEED 6.28
#define LIGHT_THRESHOLD 85
#define DELTA 10

int main(int argc, char **argv) {
  wb_robot_init(); // initialize Webots API
  
  // initialize sensors
  WbDeviceTag ps[8];
  char ps_names[8][4] = {
    "ps0", "ps1", "ps2", "ps3",
    "ps4", "ps5", "ps6", "ps7"
  };
  
  for (int i = 0; i < 8; i++) {
    ps[i] = wb_robot_get_device(ps_names[i]);
    wb_distance_sensor_enable(ps[i], TIME_STEP);
  };

  // initialize motors
  WbDeviceTag left_motor = wb_robot_get_device("left wheel motor");
  WbDeviceTag right_motor = wb_robot_get_device("right wheel motor");
  
  wb_motor_set_position(left_motor, INFINITY);
  wb_motor_set_position(right_motor, INFINITY);
  
  wb_motor_set_velocity(left_motor, 0.0);
  wb_motor_set_velocity(right_motor, 0.0);
 
  while (wb_robot_step(TIME_STEP) != -1) {
  
    // read sensors
    double ps_values[8];
    for(int i = 0; i < 8; i++) 
      ps_values[i] = wb_distance_sensor_get_value(ps[i]);  
    
    // detect relative walls  
    bool top_wall = 
      ps_values[0] > LIGHT_THRESHOLD ||
      ps_values[7] > LIGHT_THRESHOLD;
    bool side_wall = 
      ps_values[1] > LIGHT_THRESHOLD ||
      ps_values[2] > LIGHT_THRESHOLD;
    
    double adjust_magnitude = 
        ((ps_values[7] + ps_values[0])/2) // avg of front sensors
      - ((ps_values[3] + ps_values[4])/2) // avg of back sensors
    ;
    
    bool parallel = (fabs(adjust_magnitude) < DELTA); // check if adjust is less than error tolerance
    
    // adjust motor speeds to obstacles
    double left_speed = MAX_SPEED * 0.5;
    double right_speed = MAX_SPEED * 0.5;
    
    if (top_wall) {
      left_speed = 0;
    } else if (side_wall) {
      left_speed = MAX_SPEED * 0.5; //(adjust_magnitude/LIGHT_THRESHOLD); //MAX_SPEED * 0.5;
    }
    
    if (!parallel && side_wall){
      printf("adjusting left_speed by %f\n", 0.2 * (adjust_magnitude/LIGHT_THRESHOLD));
      left_speed -= 0.2 * (adjust_magnitude/LIGHT_THRESHOLD);
    }
    
    printf("side_wall = %d; top_wall = %d\n", side_wall, top_wall);
    
    wb_motor_set_velocity(left_motor, left_speed);
    wb_motor_set_velocity(right_motor, right_speed);
    
  };

  /* This is necessary to cleanup webots resources */
  wb_robot_cleanup();

  return 0;
}
