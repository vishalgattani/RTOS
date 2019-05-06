import RPi.GPIO as GPIO
import time
import pandas as pd
import numpy as np
from statemachine import StateMachine, State
import fcntl

'''
To be accomplished
	-> Cross enter not considered
	-> Mid enter not considered
'''

class RoomSensorLeft(StateMachine):
	### LEFT SIDE OF GATE(Sensor 1 & Sensor 2)
	# Defining States
	empty_left = State('00', initial=True) #1,2 off
	trigger1_left = State('10') #1 on
	trigger2_left = State('01') #2 on
	
	## ENTRY
	# From Empty to L1
	maybe_enter_left = empty_left.to(trigger1_left)
	maybe_enter_remain_left = trigger1_left.to(trigger1_left)

	# From L1 to L2
	enter_left = trigger1_left.to(trigger2_left)
	enter_remain_left = trigger2_left.to(trigger2_left)
	
	# From L2 to empty
	left_entered = trigger2_left.to(empty_left)

	# Do nothing
	maybe_enter_to_empty_left = trigger1_left.to(empty_left)

	## EXIT
	# From empty to L2
	maybe_exit_left = empty_left.to(trigger2_left)
	maybe_exit_remain_left = trigger2_left.to(trigger2_left)

	# From L2 to L1
	exit_left = trigger2_left.to(trigger1_left)
	exit_remain_left = trigger1_left.to(trigger1_left)
	
	# From L1 to empty
	left_exited = trigger1_left.to(empty_left)

	# Do nothing
	maybe_exit_to_empty_left = trigger2_left.to(empty_left)
	
	# Remain in Empty Left
	empty_to_empty_left =  empty_left.to(empty_left)

class RoomSensorRight(StateMachine):
	### RIGHT SIDE OF GATE(Sensor 6 & Sensor 7)
	# Defining States
	empty_right = State('00', initial=True) #6,7 off
	trigger1_right = State('10') #6 on
	trigger2_right = State('01') #7 on
	
	## ENTRY
	# From Empty to R1
	maybe_enter_right = empty_right.to(trigger1_right)
	maybe_enter_remain_right = trigger1_right.to(trigger1_right)

	# From R1 to R2
	enter_right = trigger1_right.to(trigger2_right)
	enter_remain_right = trigger2_right.to(trigger2_right)
	
	# From R2 to empty
	right_entered = trigger2_right.to(empty_right)

	# Do nothing
	maybe_enter_to_empty_right = trigger1_right.to(empty_right)

	## EXIT
	# From empty to L2
	maybe_exit_right = empty_right.to(trigger2_right)
	maybe_exit_remain_right = trigger2_right.to(trigger2_right)

	# From L2 to L1
	exit_right = trigger2_right.to(trigger1_right)
	exit_remain_right = trigger1_right.to(trigger1_right)
	
	# From L1 to empty
	right_exited = trigger1_right.to(empty_right)

	# Do nothing
	maybe_exit_to_empty_right = trigger2_right.to(empty_right)

	# Remain in Empty Right
	empty_to_empty_right =  empty_right.to(empty_right)	

def average(avg_list):
	total = 0
	print len(avg_list)

	for i in range(len(avg_list)):
		total = total + avg_list[i]

	total = total*1.0
	total= total / len(avg_list)

	if(total < 0.5):
		avg = 0
	else:
		avg = 1

	return avg

try:
	f = open('sensor_data.txt','w+')
	f.write('0')
	f.close()

	# Setting Up the FSM
	room_sensor_left = RoomSensorLeft()
	room_sensor_right = RoomSensorRight()
	flag_left = 0
	flag_right = 0
	
	# Setting up GPIO
	GPIO.setmode(GPIO.BOARD)
	#e1-11,e2-12,e3-13,e4-15,e5-16,e6-18,e7-22
	PIN_TRIGGER = 8
	PIN_ECHO1 = 11	
	PIN_ECHO2 = 12
	PIN_ECHO6 = 18
	PIN_ECHO7 = 22

	GPIO.setup(PIN_TRIGGER, GPIO.OUT)
	GPIO.setup(PIN_ECHO1, GPIO.IN)
	GPIO.setup(PIN_ECHO2, GPIO.IN)
	GPIO.setup(PIN_ECHO6, GPIO.IN)
	GPIO.setup(PIN_ECHO7, GPIO.IN)

	GPIO.output(PIN_TRIGGER, GPIO.LOW)
	print "Waiting for sensor to settle"
	time.sleep(2)

	# Common Variables
	x = 40.0
	ss = 17150.0
	count = 0
	prev_count = 0

	len_moving_avg = 4
	moving_average_gpio1 = []
	moving_average_gpio2 = []
	moving_average_gpio6 = []
	moving_average_gpio7 = []
	empty_count_left = 0
	empty_count_right = 0
	empty_for = 15

	while True:
		prev_count = count
		# Triggering the sensor
		GPIO.output(PIN_TRIGGER, GPIO.HIGH)
		time.sleep(0.00001)
		GPIO.output(PIN_TRIGGER, GPIO.LOW)

		# Getting data from each sensor
		time.sleep(x/ss)
		gpio1 = (GPIO.input(PIN_ECHO1)+ 1 )%2
		gpio2 = (GPIO.input(PIN_ECHO2)+ 1 )%2
		gpio6 = (GPIO.input(PIN_ECHO6)+ 1 )%2
		gpio7 = (GPIO.input(PIN_ECHO7)+ 1 )%2
		
		print "Original", gpio1, gpio2, gpio6, gpio7
		
		if(len(moving_average_gpio1) < len_moving_avg):
			moving_average_gpio1.append(gpio1)
			moving_average_gpio2.append(gpio2)
			moving_average_gpio6.append(gpio6)
			moving_average_gpio7.append(gpio7)
			print"Moving Average List 1", moving_average_gpio1
			print"Moving Average List 2", moving_average_gpio2
			print"Moving Average List 3", moving_average_gpio6
			print"Moving Average List 4", moving_average_gpio7

		else:
			moving_average_gpio1.pop(0)
			moving_average_gpio1.append(gpio1)
			moving_average_gpio2.pop(0)
			moving_average_gpio2.append(gpio2)
			moving_average_gpio6.pop(0)
			moving_average_gpio6.append(gpio6)
			moving_average_gpio7.pop(0)
			moving_average_gpio7.append(gpio7)

			print"Moving Average List 1", moving_average_gpio1
			print"Moving Average List 2", moving_average_gpio2
			print"Moving Average List 3", moving_average_gpio6
			print"Moving Average List 4", moving_average_gpio7

			gpio1 = average(moving_average_gpio1)
			gpio2 = average(moving_average_gpio2)
			gpio6 = average(moving_average_gpio6)
			gpio7 = average(moving_average_gpio7)

			print "Average",gpio1, gpio2, gpio6, gpio7
			
			## FSM LOGIC
			# Remain Empty Left
			if(gpio1 == 0 and gpio2 == 0 and flag_left == 0):
				room_sensor_left.run('empty_to_empty_left')
				empty_count_left = 0

			elif(gpio1 == 0 and gpio2 == 0 and flag_left != 0 and empty_count_left < empty_for):
				empty_count_left = empty_count_left + 1

			# Maybe Enter Left
			elif(gpio1 == 1 and gpio2 == 0 and flag_left == 0):
				flag_left = 1
				room_sensor_left.run('maybe_enter_left')
			elif(gpio1 == 1 and gpio2 == 0 and flag_left == 1):
				flag_left = 1
				room_sensor_left.run('maybe_enter_remain_left')

			# Maybe Enter Left to No Enter
			elif(gpio1 == 0 and gpio2 == 0 and flag_left == 1):
				flag_left = 0
				empty_count_left = 0
				room_sensor_left.run('maybe_enter_to_empty_left')

			# Maybe Enter Left to Enter Left
			elif(gpio1 == 0 and gpio2 == 1 and flag_left == 1):
				flag_left = 2
				room_sensor_left.run('enter_left')
			elif(gpio1 == 0 and gpio2 == 1 and flag_left == 2):
				flag_left = 2
				room_sensor_left.run('enter_remain_left')


			elif(gpio1 == 1 and gpio2 == 1 and flag_left == 1):
				flag_left = 3
				room_sensor_left.run('enter_left')
			elif(gpio1 == 1 and gpio2 == 1 and flag_left == 3):
				flag_left = 3
				room_sensor_left.run('enter_remain_left')
			elif(gpio1 == 0 and gpio2 == 1 and flag_left == 3):
				flag_left = 3
				room_sensor_left.run('enter_remain_left')

			# Enter Left to Mayber Enter Left
			elif(gpio1 == 1 and gpio2 == 0 and (flag_left == 2 or flag_left == 3)):
				flag_left = 1
				room_sensor_left.run('exit_left')

			# Entered Left
			elif(gpio1 == 0 and gpio2 == 0 and (flag_left == 2 or flag_left == 3)):
				count = count + 1
				flag_left = 0
				empty_count_left = 0
				room_sensor_left.run('left_entered')

			# Empty to Maybe exit left
			elif(gpio1 == 0 and gpio2 == 1 and flag_left == 0):
				flag_left = 4
				room_sensor_left.run('maybe_exit_left')
			elif(gpio1 == 0 and gpio2 == 1 and flag_left == 4):
				flag_left = 4
				room_sensor_left.run('maybe_exit_remain_left')

			# Maybe Exit Left to Exit Left
			elif(gpio1 == 1 and gpio2 == 0 and flag_left == 4):
				flag_left = 5
				room_sensor_left.run('exit_left')
			elif(gpio1 == 1 and gpio2 == 0 and flag_left == 5):
				flag_left = 5
				room_sensor_left.run('exit_remain_left')


			elif(gpio1 == 1 and gpio2 == 1 and flag_left == 1):
				flag_left = 6
				room_sensor_left.run('exit_left')
			elif(gpio1 == 1 and gpio2 == 1 and flag_left == 6):
				flag_left = 6
				room_sensor_left.run('exit_remain_left')
			elif(gpio1 == 1 and gpio2 == 0 and flag_left == 6):
				flag_left = 6
				room_sensor_left.run('exit_remain_left')

			# Exited Left
			elif(gpio1 == 0 and gpio2 == 0 and (flag_left == 5 or flag_left == 6)):
				count = count - 1
				flag_left = 0
				empty_count_left = 0
				room_sensor_left.run('left_exited')
			
			# Maybe Exit Left to No Enter
			elif(gpio1 == 0 and gpio2 == 0 and flag_left == 4):
				flag_left = 0
				empty_count_right = 0
				room_sensor_left.run('maybe_exit_to_empty_left')
			
			# Exit Left to Mayber Exit Left
			elif(gpio1 == 0 and gpio2 == 1 and (flag_left == 5 or flag_left == 6)):
				flag_left = 4
				room_sensor_left.run('enter_left')

			# Remain Empty Right
			if(gpio6 == 0 and gpio7 == 0 and flag_right == 0):
				room_sensor_right.run('empty_to_empty_right')
				empty_count_right = 0

			elif(gpio6 == 0 and gpio7 == 0 and flag_right != 0 and empty_count_right < empty_for):
				empty_count_right = empty_count_right + 1

			# Maybe Enter Right
			elif(gpio6 == 1 and gpio7 == 0 and flag_right == 0):
				flag_right = 1
				room_sensor_right.run('maybe_enter_right')
			elif(gpio6 == 1 and gpio7 == 0 and flag_right == 1):
				flag_right = 1
				room_sensor_right.run('maybe_enter_remain_right')

			# Maybe Enter Right to Enter Right
			elif(gpio6 == 0 and gpio7 == 1 and flag_right == 1):
				flag_right = 2
				room_sensor_right.run('enter_right')
			elif(gpio6 == 0 and gpio7 == 1 and flag_right == 2):
				flag_right = 2
				room_sensor_right.run('enter_remain_right')


			elif(gpio6 == 1 and gpio7 == 1 and flag_right == 1):
				flag_right = 3
				room_sensor_right.run('enter_right')
			elif(gpio6 == 1 and gpio7 == 1 and flag_right == 3):
				flag_right = 3
				room_sensor_right.run('enter_remain_right')
			elif(gpio6 == 0 and gpio7 == 1 and flag_right == 3):
				flag_right = 3
				room_sensor_right.run('enter_remain_right')

			# Entered Right
			elif(gpio6 == 0 and gpio7 == 0 and (flag_right == 2 or flag_right == 3)):
				count = count + 1
				flag_right = 0
				empty_count_right = 0
				room_sensor_right.run('right_entered')

			# Empty to Maybe exit Right
			elif(gpio6 == 0 and gpio7 == 1 and flag_right == 0):
				flag_right = 4
				room_sensor_right.run('maybe_exit_right')
			elif(gpio6 == 0 and gpio7 == 1 and flag_right == 4):
				flag_right = 4
				room_sensor_right.run('maybe_exit_remain_right')

			# Maybe Exit Right to Exit Right
			elif(gpio6 == 1 and gpio7 == 0 and flag_right == 4):
				flag_right = 5
				room_sensor_right.run('exit_right')
			elif(gpio6 == 1 and gpio7 == 0 and flag_right == 5):
				flag_right = 5
				room_sensor_right.run('exit_remain_right')


			elif(gpio6 == 1 and gpio7 == 1 and flag_right == 1):
				flag_right = 6
				room_sensor_right.run('exit_right')
			elif(gpio6 == 1 and gpio7 == 1 and flag_right == 6):
				flag_right = 6
				room_sensor_right.run('exit_remain_right')
			elif(gpio6 == 1 and gpio7 == 0 and flag_right == 6):
				flag_right = 6
				room_sensor_right.run('exit_remain_right')

			# Exited Left
			elif(gpio6 == 0 and gpio7 == 0 and (flag_right == 5 or flag_right == 6)):
				count = count - 1
				flag_right = 0
				empty_count_right = 0
				room_sensor_right.run('right_exited')

			# Maybe Enter Right to No Enter
			elif(gpio6 == 0 and gpio7 == 0 and flag_right == 1):
				flag_right = 0
				empty_count_right = 0
				room_sensor_right.run('maybe_enter_to_empty_right')
			
			# Enter Right to Mayber Enter Right
			elif(gpio6 == 1 and gpio7 == 0 and (flag_right == 2 or flag_right == 3)):
				flag_right = 1
				room_sensor_right.run('exit_right')
			
			# Maybe Exit Right to No Enter
			elif(gpio6 == 0 and gpio7 == 0 and flag_right == 4):
				flag_right = 0
				empty_count_right = 0
				room_sensor_right.run('maybe_exit_to_empty_right')

			# Exit Right to Mayber Exit Right
			elif(gpio6 == 0 and gpio7 == 1 and (flag_right == 5 or flag_right == 6)):
				flag_right = 4
				room_sensor_right.run('enter_right')

		# Printing count and stabalising all sensors
		
		print count," ----> ", room_sensor_left.current_state, room_sensor_right.current_state
		if(prev_count != count):
			f = open('sensor_data.txt','w+')
			while True:
				try:
					fcntl.flock(f, fcntl.LOCK_EX | fcntl.LOCK_NB)
					f.write(str(count))
					fcntl.flock(f, fcntl.LOCK_UN)
					break
				except IOError as e:
					# raise error on unrelated IOErrors
					if e.errno != errno.EAGAIN:
						raise
					else:
						time.slepp(0.1)
			f.close()

		time.sleep(0.01)

finally:
	GPIO.cleanup()
