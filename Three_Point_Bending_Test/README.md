# Three Point Bending Device


This repository includes the instructions to build a simple and affordable three point bending test machine for assessing the stiffness of a wrist exoskeleton.

| ![Base setup](Figures/ThreePointBending_Base.png) | ![Load cell setup](Figures/ThreePointBending_LoadCell.png) |
| :-----------------------------------------------: | :--------------------------------------------------------: |
|             **Figure 1:** Base setup              |               **Figure 2:** Load cell setup                |


## Introduction
The device consists of two main components:
- a base (Figure 1)
- an end effector (Figure 2)

The base was designed to securely hold the TSG during testing and to mount the servo motor in the appropriate position. Its modular design allows stiffness measurements at different points of the exoskeleton. The end effector is actuated by an MG996R servo motor, where its angular displacement is converted into a linear displacement by means of a spur-to-linear gearbox. The tip of the end effector distributes the applied force equally onto four load cells encapsulated in a 3D-printed cap. These load cells are arranged in a Wheatstone bridge configuration, enabling precise measurement of small resistance changes, which are caused by mechanical deformation under load. 

The Wheatstone bridge converts resistance variations into a differential voltage signal in the millivolt range. To process this signal, the HX711 analog-to-digital converter (ADC) was employed to amplify, filter, and digitise the output. The HX711 is a precision 24-bit ADC widely used in weighing scales and industrial control, designed specifically to interface with Wheatstone bridge sensors. It communicates with microcontrollers using a custom two-wire serial protocol (DT and SCK), accessible via the HX711\_ADC library. The HX711 continuously samples the signal and holds the converted data until clocked by the microcontroller, operating at a rate of $10~\mathrm{Hz}$.

## Required Tools and Component
- A 3D printer (FDM was used)
- Caliper
- MG996R Servo Motor
- [Four Strain-Gauge Sensors and HX711 Module](https://www.amazon.co.uk/YOUMILE-110lbs-Half-bridge-Weight-Resistance/dp/B07TWLP3X8)  

## Force Calibration

The HX711 ADC returns a unitless raw value $r$ that is proportional to the applied force $F$. Therefore, to determine the linear mapping parameters between the HX711 measurements and the applied force, a set of calibration masses in the range of $m \in [10,500]g$ were used. Assuming a gravitational acceleration of $g = 9.81~\mathrm{m/s^2}$, these masses correspond to an applied force of $F \in [0.0981,4.905] N$. The linear mapping is achieved by determining the parameters in linear equation
$$F = a + b \cdot r$$
where $F$ is the applied force in newtons, $r$ is the unit-less output from the HX711, and $a$, $b$ are the regression coefficients determined from the calibration data.

Given the calibration data vectors for applied force $\mathbf{F} = [F_1, F_2, \ldots, F_n]$ and corresponding raw sensor readings $\mathbf{r} = [r_1, r_2, \ldots, r_n]$, the linear model coefficient $\{a,b\}$ can be found using least squares method

$$
\boldsymbol{\theta} =
\begin{bmatrix}
a \\
b
\end{bmatrix}
= (\mathbf{X}^\top \mathbf{X})^{-1} \mathbf{X}^\top \mathbf{F},
\qquad
\mathbf{X} =
\begin{bmatrix}
1 & r_1 \\
1 & r_2 \\
\vdots & \vdots \\
1 & r_n
\end{bmatrix},
\qquad
\mathbf{F} =
\begin{bmatrix}
F_1 \\
F_2 \\
\vdots \\
F_n
\end{bmatrix}.
$$

where $\mathbf{X}$ is the $n \times 2$ matrix with a column of ones and a column of $r$ values, and $\mathbf{F}$ is the $n \times 1$ vector of measured forces. 

The calibration parameters $a$ and $b$ were found and used to fit the linear line, as shown in Figure 3.


|                                                            ![](force_calibration.png)                                                             |
| :-----------------------------------------------------------------------------------------------------------------------------------------------: |
| **Figure 3:** Linear regression results for loadcell calibration, relating applied force (N) with the raw HX711 sensor output values (unit-less). |



## Displacement Mapping
The base servo motor receives its commands as pulse-widths $w$ in microseconds, controlled using the `ServoTimer2.h` library.  The corresponding servo rotation is converted to linear displacement through a spur-to-linear gear mechanism, shown in Figure 2.

To identify the relationship between the servo command $w$ and the resulting linear displacement $L$, the servo was actuated with commands in the range  $w \in [1000,2000] \mu s$ and a caliper was used to measure the corresponding loadcell displacement $L$.
A linear regression model is used to determine the mapping:

$$
L = c + d \cdot w
\tag{1}
$$

where \( L \) is the linear displacement in millimeters, \( w \) is the servo pulse-width command, and \( c \), \( d \) are the regression coefficients.

Given the calibration data vectors  
\(\mathbf{L} = [L_1, L_2, \ldots, L_n]\) and  
\(\mathbf{w} = [w_1, w_2, \ldots, w_n]\),  
the coefficients \( c \) and \( d \) can be found using the least squares method:

$$
\mathbf{M}\,\boldsymbol{\delta} = \mathbf{L}, \qquad
\mathbf{M} =
\begin{bmatrix}
1 & w_1 \\
1 & w_2 \\
\vdots & \vdots \\
1 & w_n
\end{bmatrix}, \qquad
\boldsymbol{\delta} =
\begin{bmatrix}
c \\
d
\end{bmatrix}.
$$

The least–squares estimate is then given by:

$$
\boldsymbol{\delta}
= (\mathbf{M}^\top \mathbf{M})^{-1} \mathbf{M}^\top \mathbf{L}.
$$

The calibration parameters were found as $\mathbf{\delta} = [c, d]^T = [-0.0907, 0.01532]$, which achieved a good approximation of the linear mapping, as shown in Figure 4. A small sinusoidal error is observed, likely caused by the large teeth of the 3D-printed spur-to-linear gear. Using machined gears with finer teeth may improve the fit.


|                                                                ![](displacement_calibration.png)                                                                 |
| :--------------------------------------------------------------------------------------------------------------------------------------------------------------: |
| **Figure 4:** Linear regression results for loadcell calibration, relating the servo motor pulse width commands (us) with the measured linear displacement (mm). |


