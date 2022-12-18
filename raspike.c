// SPDX-License-Identifier: MIT
/*
 * Copyright (c) 2022 Embedded and Real-Time Systems Laboratory,
 *                    Graduate School of Information Science, Nagoya Univ., JAPAN
 */
#include <kernel.h>
#include "kernel_cfg.h"
#include "raspike.h"
#include "spike/pup/motor.h"

#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"

#include <pbsys/user_program.h>

#include <stdio.h>

//ToDo asp3/target/primehub_gcc/drivers/pybricks.h
extern void wup_pybricks(void);


#define DETECT_COM_FAIL_PERIOD 5000000

#define RASPIKE_PORTNO  1

int32_t color_sensor_mode = 0;
int32_t color_sensor_change = 0;

int32_t ultrasonic_sensor_mode = 0;
int32_t ultrasonic_sensor_change = 0;

int32_t gyro_sensor_mode = 0;
int32_t gyro_reset = 0;

int32_t other_command = 0;

uint32_t num_command = 0;
uint32_t num_fail = 0;

uint32_t last_command_time = 0;

uint8_t motor_reset_A = 0;
uint8_t motor_reset_B = 0;
uint8_t motor_reset_C = 0;
uint8_t motor_stop_A = 0;
uint8_t motor_stop_B = 0;
uint8_t motor_stop_C = 0;

uint8_t gyro_sensor_mode_change = 0;


#define SEND_PACKET_SIZE (sizeof("@0000:000000"))

static void
send_data(int cmd, int val)
{
    char buf[SEND_PACKET_SIZE] = "@0000:000000";
    
    buf[4] = '0' + cmd % 10; cmd /= 10;
    buf[3] = '0' + cmd % 10; cmd /= 10;
    buf[2] = '0' + cmd % 10; cmd /= 10;
    buf[1] = '0' + cmd % 10; cmd /= 10;


    buf[11] = '0' + val % 10; val /= 10;        
    buf[10] = '0' + val % 10; val /= 10;    
    buf[9] = '0' + val % 10; val /= 10;
    buf[8] = '0' + val % 10; val /= 10;
    buf[7] = '0' + val % 10; val /= 10;
    buf[6] = '0' + val % 10; val /= 10;    
    
    serial_wri_dat(RASPIKE_PORTNO, buf, SEND_PACKET_SIZE);
    
    dly_tsk(500);
}

static void
send_ack(int cmd)
{    
    send_data(cmd, 0);    
}

static void
detect_com_fail(void) {
#if 0    
    hub.display.show(hub.Image.SAD,delay=400,clear=True,wait=False,loop=False,fade=0)
    hub.led(9)
#endif      
}

void
notify_sensor_task(intptr_t exinf) {
    static uint8_t mscnt = 0;
    SYSTIM tim;

    // カラーセンサーの切り替えがあった場合、タイミングによってはget()がNoneになったり、
    // RGBではない値が取れたりするので、その場合は次の周期で通知する
      
    if (color_sensor_mode == 1) {        
#if 0
        color_val = color_sensor.get()
        if (color_val is not None) { // 暫定
            // ambient
            send_data(1, color_val[0]);
        }
#endif
    }
    else if (color_sensor_mode == 2) {
#if 0
        color_val = color_sensor.get()
        if (color_val is not None) { // 暫定
            // color
            send_data(2, color_val[0]);
        }
#endif
    }
    else if (color_sensor_mode == 3) {
#if 0
        color_val = color_sensor.get()
        if (color_val is not None) { // 暫定
            // Reflect
            send_data(3, color_val[0]);
        }
#endif
    }
    else if (color_sensor_mode == 4) {
#if 0
        color_val = color_sensor.get();
        if (color_val[0] is not None and len(color_val) == 4 and color_val[2] is not None) {
            send_data(4, color_val[0] / 4);
            send_data(5, color_val[1] / 4);
            send_data(6, color_val[2] / 4);
        }
#endif                          
    }
        
                  
    // 超音波センサー
    
    if (ultrasonic_sensor_mode == 1) {
#if 0        
        val = ultrasonic_sensor.get()[0];
        if val is None
          val = -1;
        await send_data(22, val);
#endif        
    }
    else if (ultrasonic_sensor_mode == 2) {
#if 0        
        send_data(23, ultrasonic_sensor.get());
#endif        
    }
             
    // モーター出力
#if 0    
    send_data(64, motor_rot_A.get()[0] * invert_A);
    send_data(65, motor_rot_B.get()[0] * invert_B);
    send_data(66, motor_rot_C.get()[0] * invert_C);
#endif
    
    // Gyro
#if 0    
    gyro = hub.motion.gyroscope();
    //10msec周期なので、その分を加算する
    send_data(8,int(gyro[2]));
    send_data(7,int(hub.motion.yaw_pitch_roll()[0]));
#endif

    //タッチセンサー
#if 0        
    val = touch_sensor.is_pressed();
    if (touch_sensor_value != val) {
        touch_sensor_value = val;
        sendVal = 0;
        if (touch_sensor_value) 
          // Touchセンサーは加圧のアナログ値で、2048以上をタッチとして扱うため2048とする
          sendVal = 2048;
        send_data(28,sendVal);
    }
#endif

    //Ackの処理
    //モーターリセット
    if (motor_reset_A == 1) {
        motor_reset_A = 0;
        send_ack(9);
    }
    if (motor_reset_B == 1) {
        motor_reset_B = 0;
        send_ack(10);
    }
    if (motor_reset_C == 1) {
        motor_reset_C = 0;
        send_ack(11);
    }
    if (color_sensor_change == 1) {
        color_sensor_change = 0;
        send_ack(61);
    }
    if (ultrasonic_sensor_change == 1) {
        ultrasonic_sensor_change = 0;
        send_ack(62);
    }
    if (gyro_reset == 1) {
        gyro_reset = 0;
        send_ack(13);
    }
    if (gyro_sensor_mode_change == 1) {
        gyro_sensor_mode_change = 0;
        send_ack(63);
    }
    if (motor_stop_A == 1) {
        motor_stop_A = 0;
        send_ack(5);
    }
    if (motor_stop_B == 1) {
        motor_stop_B = 0;
        send_ack(6);
    }
    if (motor_stop_C == 1) {
        motor_stop_C = 0;
        send_ack(7);
    }
    if (other_command != 0) {
        send_ack(other_command);
        other_command =0;
    }
              
    /* 100ms周期 */
    if (++mscnt == 10) {
#if 0        
            // 電流/電圧は1sec毎に送るが、位相はずらす
            if ( (long_period_count+5) % 10 ) == 0:
                await send_data(29,hub.battery.current())
            if ( (long_period_count) % 10 ) == 0:
                await send_data(30,hub.battery.voltage())
            
            #本体ボタン(100msec毎)
            button_command = 0
            if hub.button.left.is_pressed():
                button_command = 1
            if hub.button.right.is_pressed():
                button_command = button_command + 2
            if hub.button.center.is_pressed():
                button_command = button_command + 16
            if button_command != prev_button_command:
                await send_data(0,button_command)
                prev_button_command = button_command

            long_period = cur + long_period_time_us
            long_period_count = long_period_count + 1
#endif              
    }

    // 最終の有効コマンドを受け取ってから5秒経ったら、通信断絶として、画面表示を変える
    get_tim(&tim);
    if ((last_command_time + DETECT_COM_FAIL_PERIOD < tim)) {
        detect_com_fail();
    }    
}


void
wait_read(char *buf, int size)
{
    serial_rea_dat(RASPIKE_PORTNO, buf, 1);
}


void
receiver_task(intptr_t exinf) {
    uint8_t cmd;
    uint8_t data1,data2;
    uint8_t idx;
    uint8_t cmd_id;
    int32_t value;
    SYSTIM tim;
    
    while(1) {
        while(1) {
            wait_read((char*)&cmd, 1);
            if (cmd & 0x80U)
                break;
        }
        
        while(1) {
            wait_read((char*)&data1, 1);
            num_command++;
            if (data1 & 0x80U) {
                cmd = data1;
                num_fail++;
                continue;
            }
            idx = cmd & 0x7FU;

            wait_read((char*)&data2, 1);
            num_command++;
            if (data1 & 0x80U) {
                cmd = data2;
                num_fail++;
                continue;
            }
            cmd_id = idx;
            value = ((data1 & 0x1F) << 7) | data2;
            if (data1 & 0x20U)
              value = value * (-1);
            break;                
        }

        if ((value < -2048) || (value > 2048)) {
            num_fail = num_fail + 1;
            continue;
        }
        
        // 有効なコマンドを受け取った最後の時間
        get_tim(&tim);
        last_command_time = tim;
        
        // 高速化のために、motorスピードを優先して判定する
        if (cmd_id == 1) {
            // motor_A.pwm(invert_A * value) ToDo
        }
        else if (cmd_id == 2) {
            // motor_B.pwm(invert_B * value) ToDo
        }
        else if (cmd_id == 3) {
            // motor_C.pwm(invert_C * value) ToDo
        }
        else if (cmd_id == 5) {
            if (value == 1) {
                // motor_A.brake()
            }
            else {
                // motor_A.float()
            }
            motor_stop_A = 1;
        }
        else if (cmd_id == 6) {
            if (value == 1) {
                // motor_B.brake()
            }
            else {
                // motor_B.float()
            }
            motor_stop_B = 1;
        }
        else if (cmd_id == 7) {
            if (value == 1) {
                // motor_C.brake()
            }
            else {
                // motor_C.float()
            }
            motor_stop_C = 1;
        }
        else if (cmd_id == 9) {                  
            // motor_A.preset(0)
            motor_reset_A = 1;
        }
        else if (cmd_id == 10) {                  
            // motor_B.preset(0)
            motor_reset_B = 1;
        }
        else if (cmd_id == 11) {                  
            // motor_C.preset(0)
             motor_reset_C = 1;
        }
        else if (cmd_id == 61) {                  
            // Port2 Color Sensor
            // Color Sensor Mode
            // 切り替えの間カラーセンサーの取得がされると不安定になるため、modeは一時的に0にする
            color_sensor_mode = 0;
            
            if (value == 1) {
                // Ambient
                // color_sensor.mode(2)
            }
            else if (value == 2) {
                // Color
                // color_sensor.mode(0)
            }
            else if (value == 3) {                
                // Reflect
                //    color_sensor.mode(1)
            }
            else if (value == 4) {                                      
                // RGB
                // color_sensor.mode(5)
            }
            //ダミーリード
            // cv = color_sensor.get();
            color_sensor_mode = value;
            color_sensor_change = 1;
        }
        else if (cmd_id == 62) {                  
            // Port3 Ultra Sonic Sensor
            // led = b''+chr(9)+chr(9)+chr(9)+chr(9);
            // ultrasonic_sensor.mode(5,led)
            
            if (value == 1) {
                // ultrasonic_sensor.mode(0)
            }
            else if(value == 2) {
                // ultrasonic_sensor.mode(3)
            }
            // 設定のまでのWait
            ultrasonic_sensor_mode = value;
            ultrasonic_sensor_change = 1;
        }
        else if (cmd_id == 13) {                  
            // Port4 Gyro Sensor
            gyro_sensor_mode = 0;
            if (value == 1) {
                // hub.motion.yaw_pitch_roll(0)
            }
            gyro_reset = 1;
        }                    
        else {
            other_command = cmd_id;
        }
    }
}

/*
 * Main Task
 */
void
main_task(intptr_t exinf)
{
  // Prepare the pybricks runtime for running a user program.
  // TODO: integrate pbsys_user_program_prepare() and wup_pybricks into one function. 
  pbsys_user_program_prepare(NULL);
  wup_pybricks();
    
  pbio_error_t err;
  pup_motor_t *motor;
  
  dly_tsk(3000000);
    
#if 0
  // Get pointer to servo
  motor = pup_motor_get_device(PBIO_PORT_ID_A);
  if(motor == NULL)
  {

  }
  
  // Set up servo
  for(int i = 0; i < 10; i++)
  {
    bool reset_count = true;
    err = pup_motor_setup(motor, PUP_DIRECTION_CLOCKWISE, reset_count);
    if(err != PBIO_ERROR_AGAIN)
      break;
    
    // Wait 1s and try one more
    dly_tsk(1000000);
  }
  // TEST_ASSERT_NOT_EQUAL(err, PBIO_ERROR_NO_DEV);
  // TEST_ASSERT_EQUAL(err, PBIO_SUCCESS);
  
  pup_motor_set_duty_limit(motor, 30);

  err = pup_motor_set_speed(motor, 500);

  if (err != PBIO_SUCCESS)
  {
  }
#endif
    
    sta_cyc(NOTIFY_SENSOR_CYC);
    act_tsk(RECEIVER_TASK);
        
  while (1)
  {
    slp_tsk();
  }

  pbsys_user_program_unprepare();
  wup_pybricks();
}
