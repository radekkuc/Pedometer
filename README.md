# Pedometer
Pedometer using ​microcontroler FRDM-KL05Z

Description
The Pedometer allows us to easily count steps. The display shows the total number of steps taken and the number of steps per minute (SPM). Due to the limitation of a short USB cable, we need to simulate each step to ensure it is registered. To do this, we lift our board, equipped with an accelerometer, upwards, and then shake it downwards. It counts your total steps and also steps per minute.

How It Works
The results are based on readings from the accelerometer. The sum of the absolute values of the 'X,' 'Y,' and 'Z' coordinates is calculated. The data should ideally resemble a sinusoidal pattern for one step. However, due to the lack of appropriate filtering and a low sampling frequency, they more closely resemble peaks.

In the program, step detection is performed as follows:
A step is detected when the absolute value of the acceleration in any of the three axes (X, Y, Z) exceeds the defined sensitivity threshold.
Every minute, the steps per minute (SPM) is calculated based on the number of steps counted in that minute.

You can run program using programming environment Keil uVison

