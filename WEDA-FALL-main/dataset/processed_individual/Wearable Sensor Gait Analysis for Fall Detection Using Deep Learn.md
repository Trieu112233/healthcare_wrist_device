

## Old Dominion University Old Dominion University
ODU Digital Commons ODU Digital Commons
## Electrical & Computer Engineering Theses &
## Dissertations
## Electrical & Computer Engineering
## Spring 5-2023
Wearable Sensor Gait Analysis for Fall Detection Using Deep Wearable Sensor Gait Analysis for Fall Detection Using Deep
## Learning Methods Learning Methods
## Haben Girmay Yhdego
Old Dominion University, yhdego.haben@gmail.com
Follow this and additional works at: https://digitalcommons.odu.edu/ece_etds
Part of the Artificial Intelligence and Robotics Commons, Bioinformatics Commons, Biomedical
Engineering and Bioengineering Commons, and the Computer Engineering Commons
## Recommended Citation Recommended Citation
Yhdego, Haben G.. "Wearable Sensor Gait Analysis for Fall Detection Using Deep Learning Methods"
(2023). Doctor of Philosophy (PhD), Dissertation, Electrical & Computer Engineering, Old Dominion
University, DOI: 10.25777/91hp-ty43
https://digitalcommons.odu.edu/ece_etds/251
This Dissertation is brought to you for free and open access by the Electrical & Computer Engineering at ODU
Digital Commons. It has been accepted for inclusion in Electrical & Computer Engineering Theses & Dissertations
by an authorized administrator of ODU Digital Commons. For more information, please contact
digitalcommons@odu.edu.

## WEARABLE SENSOR GAIT ANALYSIS FOR FALL
## DETECTION USING DEEP LEARNING METHODS
by
## Haben Girmay Yhdego
B.S.c July 2012, Mekelle Institute of Technology, Ethiopia
M.S.c August 2017, Jean-Monet University, France
A Dissertation Submitted to the Faculty of
Old Dominion University in Partial Fulfillment of the
Requirements for the Degree of
## DOCTOR OF PHILOSOPHY
## ENGINEERING - MODELING AND SIMULATION
## OLD DOMINION UNIVERSITY
## May 2023
Approved by:
Michel Audette (Director)
Christopher Paolini (Member)
Jiang Li (Member)
Yuzhong Shen (Member)

## ABSTRACT
## WEARABLE SENSOR GAIT ANALYSIS FOR FALL DETECTION
## USING DEEP LEARNING METHODS
## Haben Girmay Yhdego
## Old Dominion University, 2023
## Director:  Dr.  Michel Audette
World Health Organization (WHO) data show that around 684,000 people die from
falls  yearly,  making  it  the  second-highest  mortality  rate  after  traffic  accidents  [1].   Early
detection of falls, followed by pneumatic protection, is one of the most effective means of
ensuring the safety of the elderly.  In light of the recent widespread adoption of wearable
sensors, it has become increasingly critical that fall detection models are developed that can
effectively process large and sequential sensor signal data.  Several researchers have recently
developed fall detection algorithms based on wearable sensor data.  However, real-time fall
detection remains challenging because of the wide range of gait variations in older.  Choosing
the appropriate sensor and placing it in the most suitable location are essential components
of a robust real-time fall detection system.
This dissertation implements various detection models to analyze and mitigate in-
juries due to falls in the senior community.  It presents different methods for detecting falls
in real-time using deep learning networks.  Several sliding window segmentation techniques
are developed and compared in the first study.  As a next step, various methods are imple-
mented and applied to prevent sampling imbalances caused by the real-world collection of
fall data.  A study is also conducted to determine whether accelerometers and gyroscopes
can distinguish between falls and near-falls.

According to the literature survey, machine learning algorithms produce varying de-
grees of accuracy when applied to various datasets. The algorithm’s performance depends on
several factors, including the type and location of the sensors, the fall pattern, the dataset’s
characteristics, and the methods used for preprocessing and sliding window segmentation.
Other challenges associated with fall detection include the need for centralized datasets for
comparing the results of different algorithms.  This dissertation compares the performance
of varying fall detection methods using deep learning algorithms across multiple data sets.
Furthermore, deep learning has been explored in the second application of the ECG-
based virtual pathology stethoscope detection system.  A novel real-time virtual pathology
stethoscope (VPS) detection method has been developed.  Several deep-learning methods
are evaluated for classifying the location of the stethoscope by taking advantage of subtle
differences in the ECG signals.  This study would significantly extend the simulation capa-
bilities of standard patients by allowing medical students and trainees to perform realistic
cardiac auscultation and hear cardiac auscultation in a clinical environment.

iv
Copyright, 2023, by Haben Girmay Yhdego, All Rights Reserved.

v
To my family, friends, and academic advisor and mentor, Dr.  Michel Audette.

vi
## ACKNOWLEDGEMENTS
I  want  to  thank  everyone  who  contributed  to  this  dissertation.   Working  with  my
advisors  and  committee  members  throughout  this  process  has  been  a  pleasure.   I  would
like to specifically acknowledge my academic advisor, Dr.  Michel Audette, who has always
supported  me  throughout  these  years.   Also,  I  would  like  to  thank  all  my  dissertation
committees, who have provided constant feedback and support.

vii
## TABLE OF CONTENTS
## Page
LIST OF TABLES . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .x
LIST OF FIGURES . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  xii
## Chapter
-   INTRODUCTION . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .1
1.1    Problem Statements  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .2
1.2    Proposed Systems  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .3
1.3    Contributions . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .4
1.4    Structure of the Dissertation . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .6
-   BACKGROUND AND RELATED WORKS . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .8
2.1    Background . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .8
2.2    Related Works  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    14
## 3.   DATA PREPROCESSING AND SLIDING WINDOW SEGMENTATION . . . . . . . . .   21
3.1    Datasets  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    21
3.2    Data Pre-processing . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    24
3.3    Sliding Window Segmentation Methods . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    26
3.4    Compensating an Imbalanced Dataset . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    30
3.5    Sensor Positions and Sensor Types . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    36
-   IMAGE BASED TRANSFER LEARNING . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .   38
4.1    Background . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    38
4.2    Wearable Sensor Signal Transforming into Images . . . . . . . . . . . . . . . . . . . . . . . .    40
4.3    Transfer Learning with Deep CNN . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    48
4.4    Summary . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    53
-   DEEP LEARNING MODELS USING RAW SENSOR SIGNALS . . . . . . . . . . . . . . . . . .   54
5.1    Background . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    54
5.2    One-dimensional Convolutional Neural Networks . . . . . . . . . . . . . . . . . . . . . . . . .    54
5.3    Attention Neural Network  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    58
5.4    Convolution Features with LSTM and Transformer Encoder . . . . . . . . . . . . . . .    65
5.5    Self-supervised Learning . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    69
5.6    Summary . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    74

viii
ChapterPage
-   RESULTS . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  76
6.1    Image Based Transfer Learning . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    77
6.2    One-dimensional Convolutional Neural Networks . . . . . . . . . . . . . . . . . . . . . . . . .    80
6.3    Attention Neural Network  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    82
6.4    Compare LSTM and Transformer Encoders  . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    84
6.5    Comparing Self-supervised LSTM and Transformer Encoders . . . . . . . . . . . . . .    85
6.6    Sliding Window Segmentation Methods . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    89
6.7    Compensating an Imbalanced Dataset . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    90
6.8    Sensor Positions   . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    91
6.9    Sensor Types  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    92
6.10  Window Size Impact . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    93
6.11  Performance Comparison  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    95
-   CONCLUSION AND FUTURE WORKS . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  99
7.1    Conclusion  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .    99
7.2Future Work  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  100
REFERENCES . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 102
## APPENDICES
## A.REAL-TIME ECG-BASED VIRTUAL PATHOLOGY STETHOSCOPE DE-
TECTION . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  116
A.1   Methodology . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  117
A.2   Results and Discussion  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  124
A.3   Conclusion  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  129
VITA . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 130

ix
## LIST OF TABLES
TablePage
1Different  types  of  Datasets  and  their  Characteristics.   *Ankle,  Chest,  Thigh,
Waist,  and  Wrist.   **Head  pelvis  upper  and  lower  thorax  and  left,  and  right
hands thighs (2) shank (2) feet (2) upper (2) and lower (2) arms.  . . . . . . . . . . . . . .  22
2Comparing the accuracy of a different combination of image encoding and CNN
architectures.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  78
3Comparing the results of different methods. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  80
4Comparing the different methods. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  81
5Comparing the different methods on the Wrist sensor signal.  . . . . . . . . . . . . . . . . . .  82
6Comparing the different model performances with existing methods. . . . . . . . . . . . .  82
7Comparing the results of different methods. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  83
8Comparison of our proposed methods against existing methods. . . . . . . . . . . . . . . . .  85
9Comparing  the  LSTM-Encoder  and  Transformer-Encoder  base  models  of  fall
detection.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  86
10Comparing  the  supervised  and  Self-supervised  methods  of  LSTM-Encoder  for
fall detection.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  86
11LSTM-Encoder results for the different segmentation methods.  . . . . . . . . . . . . . . . .  90
12LSTM-Encoder results for the various balancing methods of the dataset samples. .  91
13LSTM-Encoder results for the various sensor positions.  . . . . . . . . . . . . . . . . . . . . . . .  92
14LSTM-Encoder results for the different sensor types.  . . . . . . . . . . . . . . . . . . . . . . . . .  93
15LSTM-Encoder results for different window sizes.  . . . . . . . . . . . . . . . . . . . . . . . . . . . .  94
16Characteristics of the Standardized Patients. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  119
17Comparing the results of different DL models on test data. . . . . . . . . . . . . . . . . . . . .  127
18FCN Model performance for different SP postures . . . . . . . . . . . . . . . . . . . . . . . . . . . .  127

x
TablePage
19Comparison of the FCN model using different input features  . . . . . . . . . . . . . . . . . .  128
20Comparison of the FCN model on gender data . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  128
21Comparison of the FCN model based on BMI Data (Normal vs Overweight)   . . . .  128

xi
## LIST OF FIGURES
FigurePage
1RNN architecture. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  11
2Single cell of LSTM.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  12
3Illustration of Transformer Architecture. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  13
4Sliding window based on the maximum signal (peak values).  We use the peak
signal values calculated as the basis to define the starting point of the observation
window. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  29
5Illustration of Multi-window Sliding.  It uses a dynamic main window based on
the classes of the fall datasets (master windows:  Non-fall Large window and Fall
Large Window) and sliding conventional subwindows inside these main windows. .  31
6Illustration of the SMOTE oversampling approach. . . . . . . . . . . . . . . . . . . . . . . . . . . .  33
7Sample of CutMix and Mixup Augmentation.  Input sequence targets are in red,
and how they changed in blue.  A green value indicates a reference from another
input sequence used in the augmentation calculation. . . . . . . . . . . . . . . . . . . . . . . . . .  35
11Sample Image Transformed using Recurrence Plot. . . . . . . . . . . . . . . . . . . . . . . . . . . .  48
12Overall Model of Image-based Transfer Learning.  . . . . . . . . . . . . . . . . . . . . . . . . . . . .  50
13Proposed AlexNet Architecture.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  51
14FCN model architecture with respective filter sizes followed by a Sigmoid layer. . .  56
15The LSTM-FCN architecture with respective filter sizes- It concatenates a stan-
dard  FCN  (upper  part)  and  LSTM  (Lower  part)  and  finishes  with  a  Sigmoid
layer.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  57
16ResNet model architecture [125] with respective filter sizes to be used for pre-
training and fine-tuning by replacing FCN in the framework above. . . . . . . . . . . . . .  59
17Query, Keys and Values in Attention Mechanism. . . . . . . . . . . . . . . . . . . . . . . . . . . . .  60
18Fall detection attention network. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  63
19Architecture of the Convolutional Feature Extractor with Encoder Block: Trans-
former Encoder Block, and LSTM Encoder Block.  . . . . . . . . . . . . . . . . . . . . . . . . . . .  66

xii
FigurePage
20The self-supervised architecture. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  71
21Contrastive Learning Architecture. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  72
22Accuracy and loss over the training iteration. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  79
23Confusion Matrix for Image-based TL of the AlexNet Architecture. . . . . . . . . . . . . .  80
24Confusion  matrices  for  the  different  methods,  LSTM  [71],  CNN-LSTM  [76],
Transformer  encoder  (our  method)  and  LSTM  encoder  (our  method),  respec-
tively. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  87
25Sample predicted classes using the LSTM encoder-based model.  Non-fall activity
(class 0), fall activity (class 1), and True, Pred are the actual and predicted classes.88
26LSTM-Encoder results for different window sizes.  . . . . . . . . . . . . . . . . . . . . . . . . . . . .  95
27Performance comparison of eight datasets using six deep learning methods. . . . . . .  97
28Data processing pipeline for ECG-based stethoscope detection . . . . . . . . . . . . . . . . .  117
29Segmented ECG data from the Mitral area.  Amplitude and interval features are
extracted from the onset, peak, and offset of QRS and T waves. . . . . . . . . . . . . . . . .  120
30MLP Model Architecture.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  120
31CNN model Architecture. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  121
32LSTM Model Architecture.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  122
33CNN-LSTM Model Architecture.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  122
34Transformer Model Architecture.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  123
35Sample-based ROC curves on test data.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  125
36Subject-based ROC curves on test data.  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .  126

## 1
## CHAPTER 1
## INTRODUCTION
According to the WHO, a fall is defined as a sequence of actions from imbalance to
hitting the floor [1].  The proportion of the population older than 65 is growing worldwide.
According to the U.S. Census Bureau, more than 56 million adults ages 65 and older live in
the United States, accounting for about 16.9% of the nation’s population.  By 2030, when
the last of the baby boomer generation ages into older adulthood, it is projected that there
will be more than 73.1 million older adults [2].  One-third of the aging population (65 years)
falls each year, and all senior adults over 80 years fall annually.  USA statistics show that
about 36 million (36m) falls occur annually, and for one-fourth (28%) of the elderly over 65
years of age, falls occur yearly [3].  Of these falls, 22% of these 36m falls (8m) were injured
and needed medical treatment [3].
The death rate due to falls is high in the aging population (65+), which is half of the
injuries-related hospitalizations in this segment [1]. CDC data show that falling is the leading
cause of death in older subjects in the United States [4].  In addition to physical disabilities,
falls can cause psychological consequences, including loss of confidence and increased fears
of falling.  These avoidance behaviors may lead to isolation, deterioration, and depression
[6].  These situations require hospitalization and prolonged medical treatment.  As a result,
substantial financial costs are incurred.  Due to the imminent global aging, falls among the
elderly are of primary importance in terms of healthcare expenditures and severity.  Due to
these reasons, there is a national imperative to develop cost-effective real-time fall detection

## 2
backed by new sensor technologies and methods that are less expensive to develop and run
to help increase mobility and monitor the health of older people at home.
To use a fall detection system in the geriatric center for elderly patients, many is-
sues must be considered, depending on the technology used.  Fall detection systems can be
classified primarily into context-aware systems (camera and ambivalent sensors) and wear-
able devices [5].  Most wearable automatic sensors utilize accelerometers, gyroscopes, or tilt
sensors and set thresholds of their outputs to detect the large impact of the body with the
ground or near horizontal orientation of the trunk, or both [6].  The fall detection system
used should preserve the privacy of the patients while optimizing the flexibility and portabil-
ity of the system [7].  The wearable sensor device with accelerometer and gyroscope inertial
sensors used as the main source of data helps us avoid these issues.
In recent decades,  many researchers have developed wearable sensor-based fall de-
tection methods.  The analysis of signals obtained from wearable sensors mounted on the
human body is commonly used to monitor the health status of older patients [5].  Motion
sensors usually generate complex hip motion signals that are difficult to interpret without
expert intervention [8].  To automatically interpret sensor readings to infer the kind of hu-
man activity that a user has performed, a computationally efficient fall detection technique
is required that will provide a meaningful characterization of sensor data.
## 1.1 Problem Statements
Existing fall detection technology is designed to merely detect when an individual has
fallen and alert medical personnel or caregivers in an emergency.  It is particularly critical
for  the  elderly  and  those  with  disabilities  who  may  be  at  risk  of  falling.   Early  forms  of

## 3
fall detection relied on the user’s motion sensors and accelerometers.  These sensors would
detect sudden changes in movement and acceleration, which could indicate that the wearer
had  fallen.   However,  these  early  forms  of  fall  detection  were  not  reliable,  as  they  often
produced false alarms.  The commercial devices, the HipSafe and HipGuard, are developed
based on logic in use for cycling and equestrian sports, which have a far simpler use-case
where a users transitions from linear to angular momentum.  An example of the ambiguity
that characterizes geriatric fall detection involves a subject sitting down quickly into a chair,
which could easily lead to a false positive for such a naive detection system.
This dire situation has led to the need for real-time fall detection systems [6],  the
long-term  goal  of  which  is  to  trigger  the  instantaneous  deployment  of  injury  mitigation
mechanisms:   i.e.:   an  airbag.   With  these  developments,  robust  real-time  fall  detection
is  essential  for  the  timely  deployment  of  pneumatic  protection  actuation.   Fall  detection
systems  should  respond  in  real-time  and  be  sensitive  and  specific  enough  to  detect  falls.
This dissertation aims to explore deep learning methods to detect falls based on inputs from
wearable accelerometry sensors.
## 1.2 Proposed Systems
Deep learning algorithms have been applied to several areas, such as computer vi-
sion, image recognition, human activity recognition, and fall detection systems [9]–[11].  In
addition, deep neural networks such as Long Short Term Memory (LSTM) and Transform-
ers,  which  consider  the  temporal  and  spatial  relationship  across  the  data,  have  achieved
excellent results in several sequential tasks [12], [13].
Real-time fall detection using a wearable sensor remains a challenging problem due

## 4
to high gait variability.  Furthermore, finding the type of sensor to use and the optimal loca-
tion of the sensors are also essential factors for real-time fall detection systems.  This work
presents real-time fall detection methods using deep learning models.  First, we developed
a  fall  detection  methodology  that  combines  transfer  learning  with  2D  images  of  tri-axial
accelerometer sensor-based data to classify fall and non-fall data sets in conjunction with
pre-trained CNN. The transfer learning method is used for the purpose of overcoming the
difficulties of training a deep CNN resulting from limited fall datasets by transferring knowl-
edge learned from a large amount of existing Imagenet datasets imagenet.  However, image
encoding methods are less efficient for fall detection and cannot be deployed fast enough to
trigger an airbag.  Second,  we developed and compared different data segmentation tech-
niques for sliding windows. Next, we implemented various techniques to balance the datasets
because collecting fall datasets in the real-time setting has imbalanced nature.
Moreover,  we  designed  a  deep  learning  model  that  combines  a  convolution-based
feature  extractor  and  deep  neural  network  blocks,  the  LSTM  block,  and  the  transformer
encoder block, followed by a position-wise feedforward layer.  We found that combining the
input sequence with the convolution-learned features of different kernels tends to increase
the performance of the fall detection model.  Last, we analyzed that the sensor signals col-
lected by both accelerometer and gyroscope sensors can be leveraged to develop an effective
classifier that can accurately detect falls, especially differentiating falls from near-falls.
## 1.3 Contributions
This dissertation aims at addressing the following objectives:

## 5
•Aim 1:Developed a fall detection methodology that combines pre-trained CNNs with
triaxial accelerometer sensor-based 2D images to classify fall and non-fall data sets.
We apply several methods to convert the sensor data to RGB images, suitable for the
input of our CNN architectures.  A transfer learning method uses knowledge learned
from Imagenet datasets [14] to overcome the challenges of training a deep CNN caused
by limited fall datasets.
•Aim 2:Developed  and  compared  several  data  segmentation  techniques  for  sliding
windows.  Multi-window slide segmentation helps to develop real-time fall detection
with 0.2 seconds window size input signal. In addition, we compared various techniques
to balance the datasets because collecting fall datasets in the real-time setting leads
to imbalanced sampling.
•Aim 3:Develop real-time fall detection methods using deep learning models.  This
research designed a deep learning model that combines a convolution-based feature ex-
tractor and deep neural network blocks, the LSTM block, and the transformer encoder
block, followed by a position-wise feedforward layer.  Combining the input sequence
with the convolution-learned features of different kernels tends to increase the perfor-
mance of the fall detection model.  The sensor signals collected by accelerometer and
gyroscope sensors can be leveraged to develop an effective classifier that can accurately
detect falls, especially when differentiating falls from near-falls.
•Aim 4:Develop a performance comparison based on six types of deep learning meth-
ods to test on different datasets mounted on various parts of our body.  To our knowl-
edge,  there  is  no  research  on  fall  detection  using  machine  learning  algorithms  that

## 6
compare the different research findings across datasets.  To overcome this limitation
in the research community, this research proposes a performance comparison based on
six types of deep learning methods to test on different datasets mounted on several
parts of our body.
•Aim 5:Deep learning has been explored in the second application of the ECG-based
virtual pathology stethoscope detection system.  A novel real-time virtual pathology
stethoscope (VPS) detection method has been developed.  Several deep-learning meth-
ods are evaluated for classifying the location of the stethoscope by taking advantage
of subtle differences in the ECG signals.  This work investigated the accuracy and va-
lidity of a deep learning-based virtual pathology stethoscope detection system.  This
study would significantly extend the simulation capabilities of standardized patients
by allowing medical students and trainees to perform realistic CA and hear CA in a
clinical environment.
1.4 Structure of the Dissertation
The remaining chapters of this dissertation are organized as follows.  Chapter 2 ex-
plains the background that describes the relevant topics that will be discussed in the disser-
tation and their related works.  Chapter 3 presents the importance of data pre-processing,
balancing  the  datasets  techniques  and  feature  extraction  using  different  sliding  windows.
Chapter 4 outlines fall detection using image-based transfer learning.  Chapter 5 presents
the detection of falls using deep learning models with raw signal sensors.  Chapter 6 ana-
lyzes and discusses the results of the different methods.  Finally, Chapter 7 concludes the

## 7
dissertation results and sets the future work proposed for the dissertation.

## 8
## CHAPTER 2
## BACKGROUND AND RELATED WORKS
## 2.1 Background
2.1.1 Machine Learning and Deep Learning
Neural  Networks  (NNs)  are  computational  models  that  simulate  the  behavior  of
biological  neurons  using  an  extensive  collection  of  simple  neural  units.   Neural  units  are
connected,  and  adjoining  neurons’  activation  states  can  be  enhanced  or  inhibited  using
weight  values.   The  signal  propagates  to  other  neurons  only  after  it  exceeds  each  unit’s
threshold function or limiting function.  The network adjusts its weights during the learning
phase to predict the correct class label for input tuples.
Neural networks are a type of machine learning algorithm composed of interconnected
nodes (or neurons) that work together to process and analyze data, making them particularly
useful  for  tasks  such  as  image  and  speech  recognition,  natural  language  processing,  and
predictive analytics [12], [15]–[17].
Several parameters, such as the network layers, are best determined empirically when
training neural networks.  Network parameters are characterized by poor interpretability.
For  example,  a  network’s  learned  weights  and  hidden  units  are  difficult  to  interpret  by
humans.   A  neural  network  can,  however,  classify  patterns  on  which  they  haven’t  been
trained and are highly tolerant of noisy data.  This approach has succeeded on a wide array

## 9
of real-world data [12], [18], [19], even when attributes and classes are poorly understood
## [20].
The concept of neural networks has been around since the 1940s when Warren Mc-
Culloch and Walter Pitts introduced the first mathematical model of a neural network [21].
However, it wasn’t until the 1980s that neural networks gained popularity thanks to the de-
velopment of backpropagation, a method for training neural networks [22].  Backpropagation
is an algorithm used to train artificial neural networks.  It is a supervised learning method
that works by minimizing the error between the predicted output of the network and the
actual output.  The algorithm is called ”backpropagation” because it involves propagating
the error back through the network layers to update the neurons’ weights [22].
Backpropagation allowed neural networks to be trained on large datasets, leading to
significant advancements in computer vision and speech recognition.  However,  the popu-
larity of neural networks waned in the 1990s as other machine learning methods, such as
support vector machines and decision trees, gained prominence [22].  In the 2010s, the rise
of big data and advances in computing power led to a resurgence of interest in neural net-
works.  This was particularly true with the development of deep learning, a neural network
with multiple layers of interconnected nodes.  Deep learning has produced state-of-the-art
results in a wide range of applications, including image and speech recognition and natural
language processing [12], [15], [17].
2.1.2 A Convolutional Neural Network (CNN)
A  convolutional  neural  network  (CNN)  is  an  artificial  neural  network  specifically
designed for processing images and other grid-like structures, such as video and audio.  In

## 10
the early 2010s, large datasets, powerful hardware, and improved training algorithms made
them popular.  They were first proposed in the 1980s [23].
A CNN consists of a hierarchy of interconnected nodes that hierarchically process
data.  A convolution filter is applied to the input image to extract local features like edges,
corners, and textures.  As the layers are combined, more complex patterns are formed, and
a prediction is made in the end.
CNNs are known for automatically learning useful features from raw data.  In order
to achieve this, backpropagation is used, which lets the network adjust its weights depending
on how accurate its predictions are compared to the true labels.
Images  have  been  classified,  objects  spotted,  segments  analyzed,  and  even  artistic
styles  transferred  using  CNNs  [24].   In  addition,  convolutional  recurrent  neural  networks
(CRNNs) are used for speech recognition and natural language processing [25], with varia-
tions such as convolutional attentional recurrent NN [26].
2.1.3 Recurrent Neural Network (RNN)
Deep neural networks are powerful for pattern classification.  A feed-forward neural
network, or multilayer perceptron (MLP), is a computational model that processes informa-
tion through a series of interconnected computational nodes.  These computational nodes
are  grouped  into  layers  and  are  associated  with  one  another  using  weighted  connections.
A  limitation  of  the  MLP  architecture  is  that  it  assumes  that  all  inputs  and  outputs  are
independent  of  each  other.   In  order  for  an  MLP  to  model  a  time  series  (such  as  a  sen-
sor signal), it is necessary to include some temporal information in the input data.  RNNs
are specifically designed to tackle this problem, using recurrent connections in every unit.

## 11
Recurrent  neural  networks  (RNN)  can  encode  and  process  sequential  data,  unlike  classic
machine learning algorithms such as support vector machines (SVM).
Moreover, classic machine learning algorithms like Support Vector Machine (SVM)
process their data without any notion of sequential order, but the RNN has the ability to
encode and process sequential data.  RNNs, as shown in Figure 1 are neural networks making
use of recurrent connections in every unit.  In RNN, the activation of a neuron is fed back
to itself with a weight and a unit time delay.  It provides a memory (hidden values) of past
activations, allowing it to learn the temporal dynamics of sequential data.  The number of
layers that are traversed by backpropagation grows dramatically.  As a result of the large
number of layers, using RNN for training accelerometry data introduces a vanishing gradient
problem.  Recurrent neural networks (RNNs) amplify this problem because they are trained
by backpropagation through time.
Fig. 1.  RNN architecture.
Long  short-term  memory  networks  (LSTM)  are  recurrent  networks  that  include  a

## 12
memory component to model temporal dependencies in time series problems.  LSTM archi-
tecture avoids the problem of vanishing gradients by introducing forget gating (as shown in
Figure 2) [27], [28].  The recurrent LSTM cell is an extension of RNNs, where the internal
memory can be updated, erased or read out as shown in Figure 2.
Fig. 2.  Single cell of LSTM.
This dissertation expands on this theme of LSTM in chapter 5 through gait analysis
for fall detection method based on accelerometry signal input.  As a fall event is a sequence
of actions from imbalance to hitting the floor, its temporal correlation takes advantage of
RNNs, which have the ability to encode and process sequential data, such as acceleration
measurements from wearable sensors.
## 2.1.4 Transformer
As a result of their ability to sequentially process sequential data, recurrent neural
networks (RNNs) are widely used in natural language processing (NLP). Modeling long-term

## 13
dependence on sequential data has proven particularly effective with LSTM- and GRU-based
RNNs.  The RNN, however, has a significant disadvantage.  Since they are sequential, they
are computationally expensive and time-consuming to train, especially for long sequences
## [12].
## Nx
Multi-Head
## Attention
## Feed Forward
## Linear
## Add & Norm
## Add & Norm
Input EmbeddingOutput Embedding
## Nx
## +
## ~
## Positional
## Encoding
Multi-Head
## Attention
## Feed Forward
## Add & Norm
## Add & Norm
## +
## ~
## Inputs
Outputs (Shifted right)
## Positional
## Encoding
Multi-Head
## Attention
## Add & Norm
SoftMax
## Outputs Probability
Fig. 3.  Illustration of Transformer Architecture.
Furthermore, LSTM networks do not work well for lengthy sequences because such

## 14
sequences  have  a  single  latent  vector  as  output.   It  means  that  the  last  LSTM  unit  may
not  be  able  to  capture  the  total  essence  of  the  sequence.   Since  all  the  input  segments
of the lengthy sequence are captured into one vector, if an output segment depends on a
specific input segment,  then proper attention is not given to it in a simple LSTM model
[12].  To address these issues, NLP introduced Transformers [12], [29], [30], a comprehensive
approach to sequence modeling.  Unlike RNNs, transformers do not process the input data
in order; rather, they focus on parts of the input sequence that are most relevant to the task.
This attention mechanism is beneficial for language modeling and other NLP tasks since it
captures the sequence’s global context and long-range dependencies [31], [32].  Transformer,
as shown in Figure 34, was first proposed by Vaswani et al.  [12].
Transformers  use  a  self-attention  mechanism  instead  of  convolutional  or  recurrent
neural  networks  to  process  input  sequences.   Because  of  this  approach,  this  architecture
is  highly  parallelizable  and  scalable  to  much  longer  sequences  than  was  previously  pos-
sible.   The  Transformer  architecture  transforms  input  sequences  into  vectors,  which  are
subsequently passed through feed-forward neural networks and layers of self-attention.  By
selectively focusing on different parts of each layer of the sequence,  self-attention models
can capture long-range dependencies and relationships between different elements of input
sequences.   A  number  of  natural  language  processing  tasks  have  been  achieved  with  the
Transformer, including machine translation [30], language modeling [29], and question an-
swering [33].  Time series data analysis [34] can also be performed using it in addition to
image captioning [35] and speech recognition [36].

## 15
## 2.2 Related Works
Many research efforts have been devoted to the development of efficient and cost-
effective fall detection systems (FDS’s).  FDS’s helps us to differentiate falls from activities
of daily living (ADL) where currently an alert to a remote monitoring point is automatically
emitted as soon as the patient falls [37].  Commonly, fall detection systems are categorized
into two classes depending on the deployed sensor technology: wearable sensors and ambient
sensors (such as cameras, vibration, or infrared sensors).  The ambient–based system, which
is  expensive,  is  restricted  to  a  particular  pre-defined  area  and  also  affects  the  privacy  of
patients caused by audiovisual sensors.  Considering those limitations, wearable FDSs offer
a cheaper alternative to detect falls based on one or several wearable sensors attached to
the user’s body or clothes so that they can be ubiquitously transported.  where currently an
alert to a remote monitoring point
In [38]–[40] studies, the authors introduce inertial measurement units (IMU), with an
accelerometer, gyroscope, and magnetometer sensors, in the human motion analysis.  They
present the research community with a wide range of possibilities in the assessment of gait.
A kinematic approach was used to estimate 2D and 3D joint kinematics, initial, and terminal
contact events.  Based on these events, twelve spatiotemporal parameters were calculated.
But, it appears that the spatiotemporal parameters calculated from the IMU sensors need
further  improvement.   In  study  [41],  Li  presented  a  fall  detection  algorithm  that  tries  to
reduce both false positives and false negatives caused by two activities,  sitting down fast
and falling on stairs.  He developed fall detection by using gyroscopes and accelerometer-
derived posture information from these two activities. Even though he gets better specificity,

## 16
and sensitivity for these two activities (sitting down fast and falling on stairs), the overall
specificity and sensitivity of this method of classification have less specificity and sensitivity,
while also failing in some activities, like getting into bed.
Activity-related pattern recognition has elicited academic research and industry over
the last decades.  Fall detection is an example of activity recognition that has garnered inter-
est; several methods have been developed with varying levels of accuracy [6], [42], [43][44],
[11]).  Some methods use only wearable inertial sensors [45]–[47], while others combine cam-
eras [48] and ambient sensors [49] deployed in a living environment [50].  Methods of fall
detection vary; however, the most common implementations involve either thresholding or
machine learning [51].
In  the  simple  thresholding  method,  threshold  values  of  specific  parameter  magni-
tudes, calculated from sensor data such as tri-axial acceleration, are used to detect a fall.
Automatic  fall  detection  using  a  threshold-based  method  of  single  parameters  calculated
from accelerometry measurements has a high sensitivity, but a relatively low specificity [52].
For instance, Purwar [11] uses a tri-axial accelerometer to set thresholds of acceleration and
orientation of the torso through experiments to detect falls, which achieved an accuracy of
81% [11].  Alternatively, to calculate the torso’s angular position and acceleration prior to
thresholding, Bourke implements a bi-axial gyroscope to detect fall events [43].
Meanwhile, Shi et al.  proposed an airbag system to protect the hip during falls in a
manner comparable to airbags deployed in a car crash [53],[54].  To ensure that the airbag
was fully filled before a collision, they applied a tri-axial accelerometer and gyroscope, then
set accelerations and angular velocities for thresholding while using Support Vector Machine

## 17
(SVM) methods as a classifier to detect the fall events.  However, accuracy and throughput
were not assessed [53], [54].
Montesinos [55], and Klenk [56] have thoroughly compared the performance of basic
thresholding techniques with a wide set of supervised-based learning solutions (mainly SVM
and CNN). It is difficult to use threshold techniques as we are deciding the average range of
fall and non-fall activity for ambiguous cases that are common in geriatrics.  Although CNN
networks  were  mainly  used  for  image  feature  extraction,  CNN  1D  has  also  been  used  in
some recent works for fall detection systems of inertial sensor signals and action recognition
using wearable sensor signals [57], [58].  Those papers mainly use 1D raw inertial sensors as
inputs.
Other approaches, like Dai [59] and Lee [60], proposed thresholding both the mag-
nitude and the vertical projection of the acceleration.  As a result,  when both thresholds
are exceeded within a time window,  a fall is detected.  The latter paper by Lee [60] uses
the magnitudes of angular velocity and acceleration as input data.  When both magnitudes
are greater than the specified thresholds, a fall is detected.  In related work, Viet uses both
upper and lower thresholds for the acceleration magnitude [61].  If the upper threshold is
exceeded in less than 1s, a fall template using a wavelet transform is used for comparison
with the acceleration signal.  A fall is detected if the comparison produces a high similar-
ity value.  Even though most of these methods depend on the threshold,  it is difficult to
use threshold methods because of ambiguous cases that are common in geriatrics.  Due to
this limitation, recent approaches [62] employ feature extraction engineering and machine
learning classifiers to improve detection accuracy.

## 18
Many researchers have developed classical machine learning-based fall detection sys-
tems [63].  Different classifiers methods are used with hand-crafted feature extractions for
real-time fall detection systems, such as logistic regression [64], Naive Bayes [64], [65], deci-
sion tree [64], [65], support vector machines [64], and k-nearest neighbors [64]–[66].
The machine learning implementations differ in many aspects. Some of these methods
like [67] and [61], combine both thresholding and machine learning methods.  Tong’s work
[68] uses wearable sensors to collect the data, non-overlapping sliding windows to extract the
features, and while combining Hidden Markov Models (HMM) and thresholding to classify
them.  This result shows perfect specificity (true positive rate) and sensitivity (true negative
rate)  for  unambiguous  fall  cases.   Even  though  HMM  is  a  sequential  model  that  takes
advantage of time series data of fall events, the result does not indicate that the algorithm
is sufficiently efficient to be used in real-time.  Also, the data collected is not really falling
situations of geriatric subjects but simulated data from youthful subjects.  Moreover,  the
dataset does not contain near-fall events, which is characteristic of the geriatric population.
Many fall detection algorithms suffer in classifying near-fall and fall events.  Furthermore,
Tong’s method employs thresholding to boost performance;  however, thresholding cannot
cope with ambiguous cases that are common in geriatrics, and it will wrongly classify near-
fall events in real-time fall detection.
Deep learning algorithms have been applied to several areas, such as computer vision
[69], image recognition [70], human activity recognition [10], and fall detection systems [71].
Fall detection system development mainly uses simulated fall data sets by well-protected
non-patient subjects to be then used and validated by real patients [56], [72]. A fall detection

## 19
system based on simulated falls of such young subjects was developed by [73].  Other fall
detection devices based on real patients are developed by [74], but it has false positives and
false negatives.
Recurrent neural networks (RNN) and long short-term memory (LSTM) are popular
algorithms used in sequence models to encode and process sequential data.  As a wearable
sensor-based fall detection method generates sequential datasets, they have the advantage
of using these sequential models.  Sensor fusion of accelerometer and gyroscope data streams
using a hybrid CNN-LSTM method proposed by Ruben [75] and LSTM-based activity recog-
nition by Aicha [76] are used for fall recognition.  The Aicha [76] study employs a single
inertial sensor placed on the trunk.  The raw acceleration and angular velocity signals are the
Aicha inputs to the CNN and LSTM models.  However, Ruben’s approach uses KNN clas-
sifiers, which may be difficult to apply to real-time fall detection due to the computational
time of distance computation between the different features.
Sequentially aware deep neural networks such as RNN and LSTM, which consider
the temporal and spatial relationship across the data, have achieved better results in several
sequential tasks.  Despite the progress of these models in time series data, parallel processing
for the layer outputs is not possible, a limitation that makes it difficult to learn long-range
dependencies.  In the last decade, researchers proposed attention-based models for natural
language processing (NLP) tasks,  and they achieved state-of-the-art results with reduced
computation time [77].  Attention networks and related architectures have led to improve-
ments  in  several  sequential  data  to  problems  comparable  to  our  fall  detection  use  cases
[77].  However, these models have not been used for fall detection to our knowledge.  This

## 20
paucity is likely due to several factors, such as difficulty of accurately encoding positional
information, focus on point-wise values, and lack of research on handling multivariate input
features.  Additionally, outside of natural language processing, attention neural networks are
still relatively novel and limited in their adoption by the fall detection community.  We use
the attention neural network for the fall detection problem in order to achieve a competitive
specificity and sensitivity performance.
Nowadays, several machine learning algorithms are used to further improve the detec-
tion rate accuracy [71], [75], [78].  There are many ways to increase fall detection accuracy;
the best-known improvements are to collect the data, extract features carefully, and select
the optimal algorithms for classification.  Extracting features from the input sensor signals
to the machine learning techniques impacts the detection algorithms’ accuracy [79].  Many
feature extraction methods exist for signal data; the sliding window is a widely used method
[42].  Several studies show how window size and type impact the detection rate [42], [79].
A method proposed by Edy [42] compares the impact of fixed-size non-overlapping sliding
windows  (FNSW)  and  fixed-size  overlapping  sliding  windows  (FOSW)  for  fall  detection.
This study uses two publicly accessible datasets:  the Cogent and SisFall datasets [80], [81]
collected with wearable sensors, while supervised machine learning algorithms of K-Nearest
Neighbors  (KNN)  and  Support  Vector  Machine  (SVM)  are  used  classifiers.   The  results
differ according to different datasets:  the FOSW does better with the Sisfall dataset [81],
and FNSW performs better with the Cogent datasets [80].  In addition, Edyy [42] does not
assess the effect of window size and type on the performance of each classifier.

## 21
## CHAPTER 3
## DATA PREPROCESSING AND SLIDING WINDOW
## SEGMENTATION
## 3.1 Datasets
Researchers have collected and used several publicly available fall datasets to develop
and evaluate fall detection systems.  Some of the most commonly used datasets that are used
for this dissertation and their characteristics are shown in Table 1.
3.1.1 SDSU Dataset
Paolini conducted fall experiments on well padded human subjects in San Diego State
University’s Neuromechanics and Neuroplasticity laboratory.  He collected the fall dataset
with  a  sampling  rate  of  100  HZ  [82]  based  on  sensors  mounted  on  different  parts  of  our
body.  The laboratory is equipped with a wireless 3D motion to capture cameras that record
human subject movements [83] for annotating the labeled data.  The data are collected from
sixteen subjects between the age of 20-50 years-two of them are females, and the rest are
males.  The Noraxon myoMotion research inertial measurement unit (IMU) sensors measure
features  such  as  acceleration  and  angular  velocity.   The  datasets  contain  activities  such
as near-fall,  backward and forward falls,  obstacle-based fall,  and ADL (activities of daily
living).

## 22
TABLE 1.  Different types of Datasets and their Characteristics.
*Ankle, Chest, Thigh, Waist, and Wrist.
**Head pelvis upper and lower thorax and left, and right hands thighs (2) shank (2) feet
(2) upper (2) and lower (2) arms.
DatasetsAge
## Sampling
## Rate
(Hz)
Number of
## Position
## Near
## Falls
## Fall
## Class
## (in %)
## Number
of Sensors
per Position
Number of
## Subjects
## (M/F)
SisFall [81]19-75  200
## 1
(Waist)
Yes   44A, A, G38 (19/19)
## Cogent
## Labs [80]
## 18-51  100
2 (Chest
and Thigh)
Yes   43A, G42 (36/6)
## DLR [84]23-52  100
## 1
(Waist)
No    6.25A, G, M19 (11/6)
TST Fall
detection [85]
## 22-39  100
2 (Waist
and Wrist)
No    50A11 (N/A)
UniMiB
## SHAR [86]
18-60  501 (Thigh)No    47A30 (6/24)
UR Fall
## Detection [44]
## 26-256
1 (Waist
## (near Pelvis))
No    44A6 (6/0)
UMAFall [87]   18-55  1005 (*)No    27A, G, M17 (10/7)
## SDSU
## Dataset [71]
20-50  10016 (**)Yes   13A, G, M16 (14/2)
Each subject walked straight (back and forth) while wearing the VR headset to a
path in the laboratory.  The VR headset would depict a straight sidewalk for the participant
with no obstacles or deviations in the middle of the track, ensuring that the participant is
familiar with his/her virtual environment while walking.  Mattresses were placed at the front
and along the side of the path to prevent injuries.  Upon falling, the sensors are checked for
dislocation and adjusted accordingly before performing the next fall experiment.
The Noraxon myoMotion research inertial measurement unit (IMU) sensors are used
to  measure  accelerometer  and  gyroscope  features.    These  feature  values  were  saved  as
comma-separated value (CSV) files accessed using the Noraxon MR3 Software and exported
to the computer [71].  A clean fall is considered a proper fall event, labeled with a binary

## 23
value of ’1’ in each test case.  At the same time, non-fall activities (standing, walking) and
near-fall activities (slipping, stumbling, and sitting) are non-fall events labeled with a binary
value ’0’.  Our deep learning models use these CSV files of different human subjects as train
and test datasets.
## 3.1.2 Sisfall Dataset
A publicly Sisfall [81] dataset used consists of 19 ADLs and 15 falls performed by 23
adults (11 male and 12 female) and 15 (8 male and 7 female) elders.  The data is collected
using waist mounted embedded device with two accelerometers and one gyroscope.  For the
methods  in  this  dissertation,  we  use  one  set  of  accelerometer  and  gyroscope  data,  which
consists of 3D acceleration data and 3D angular velocity.  The data is sampled at a rate of
## 200 HZ.
3.1.3 URFD Dataset
A publicly available UR fall detection dataset URFD [44], [88] has been used.  This
dataset contains 70 sequences of which 30 involve falls and 40 consist of non-fall activities.
Kepski et al.  recorded these datasets from two accelerometers placed at the waist and depth
images collected using two Microsoft Kinect cameras.  For our proposed method, we use the
accelerometry data only, which consists of 3D acceleration dataa(t) = [a
x
## (t),a
y
## (t),a
z
## (t)].
This dataset is sampled at a rate of 256 HZ.
3.1.4 UMA Dataset
UMA  fall  detection  dataset  [87]  contains  videos  of  11  elderly  subjects  falling  and

## 24
performing ADLs.  In addition to RGB video streams, accelerometers and gyroscopes are
included  in  the  dataset.   Researchers  at  the  University  of  Malaga  in  Spain  collected  the
dataset. Each video lasts between 20 and 30 seconds, and there are 130 videos in the dataset.
A Microsoft Kinect camera was used to record videos of subjects performing ADLs, such as
walking, sitting, picking up objects, and simulating falls.  The falls were induced by having
the subjects step on a foam cushion or by having them slip on a mat.  Wireless sensors were
attached to the subject’s lower back to capture accelerometer and gyroscope data.  A 50 Hz
sampling rate was used to sample acceleration and angular velocity values.
3.1.5 TST Dataset
TST fall detection dataset [85] contains videos of falls and activities of daily living
(ADLs).  Researchers at the Tokyo Institute of Technology collected data from RGB and
depth video streams, accelerometers, and gyroscopes.
3.1.6 UniMiB SHAR Dataset
In total, the UniMiB SHAR dataset [86] contains 7013 sequences, 1699 of which are
falling.  There are 51 samples in each sequence, recorded at a sampling rate of 50 Hz from
30 different volunteers.  Sensors are placed in the subjects’ pockets.  The dataset contains
traces captured from the movements of 18- to 60-year-old subjects.
## 3.2 Data Pre-processing
First,  the  datasets  are  pre-processed  to  remove  high-frequency  noise.   The  signal
noise comes from the sensor measurement or the movement of the sensor.  To remove the

## 25
noise signal, a first-order low-pass Butterworth filter [81], [89] is used.  Despite the fact that
the second-order Butterworth filter provides better results, the first-order low pass filter is
selected because it requires fewer computations and is efficient enough to insulate from noise
fall activity-related signals.
Feature normalization is a data preprocessing method that is used to set the different
feature values in a similar range.  Variations in accelerometers and gyroscopes are unlikely
to be equivalent.  Furthermore, feature normalization helps us converge our model early and
ensure that each feature’s contribution is equivalent [22], [90], [91].  Thus, it is necessary to
normalize the sensor measurements that have been selected as input to our model. It appears
that the way of normalizing the features plays an important role in the overall success of the
performance of our algorithm [90], [91].  In this approach, the data points for each feature are
normalized using the minimum and maximum of each feature.  The feature normalization
preprocessing is performed on the 3D acceleration signalsA(t) = [A
x
(t),A
y
(t),A
z
(t)] and
3D gyroscope signalsG(t) = [G
x
(t),G
y
(t),G
z
(t)].  The general formula is given as follows:
d
## Feat=
Feat−min(Feat)
max(Feat)−min(Feat)
## (1)
where  Feat  is  the  vector  of  features  x,  y,  and  z  of  the  accelerometer  and  gyroscope,  and
d
Featis the normalized feature vector calculated using the maximum and minimum values
of the vector of features.
First,  the data points of each feature (Feat)- x,  y,  and z of the accelerometry and
gyroscope are normalized (
d
Feat) using the maximum and minimum values of the features,
as shown in the equation above.

## 26
Most real-time fall detection applications should respond within less than 0.2 seconds
[92].  Therefore, segment every 0.2 seconds for each label (labeled data only) using a fixed-
size overlapping sliding window.  The size window overlapping (stride) is 0.1 seconds.  If we
get a single row of falls in this 0.2-second window signal, this window is labeled fall.
## 3.3 Sliding Window Segmentation Methods
The collected datasets have a variable temporal duration of the sequence since each
subject’s data collection time period is different.  Although most research papers [42] use
more than 2-second sliding window segmentation in their method, real-time fall detection
applications should respond in 0.2 seconds or less [92].  Therefore, several data segmentation
techniques have been implemented to detect falls in real time.  This dissertation compares
three methods of window segmentation: conventional sliding window, peak detection sliding
window, and dynamic multi-window sliding.
## 3.3.1 Conventional Sliding Window
Segmentation of signal data plays a critical role in signal recognition, and prediction
[42], [79].  A sliding window approach is used to segment the continuous time series dataset
into  short  segments.   Many  researchers  [42]  [75]  [76]  use  the  sliding  window  approach  to
the  features  of  fall  datasets.   There  are  two  methods  of  sliding  window  segmentation  for
fall detection:  fixed-size non-overlapping sliding windows and fixed-size overlapping sliding
windows.   We  use  fixed-size  overlapping  sliding  windows  because  this  approach  helps  us
generate  more  data.   The  fixed-size  overlapping  sliding  window  technique  processes  the
signal data along the temporal axis based on window size and stride size (step size).  Here,

## 27
we segment the sample signal into a 0.2-second sample signal for each label using a fixed-size
overlapping sliding window.  The size of the overlap is 0.1 seconds.  If we get a single row of
falls in this 0.2-second window signal, this window is labeled fall.
In the case of windows that do not coincide with fall segments, annotation becomes
challenging.  Such cases can be annotated using either of the following two methods:  peak
detection windowing and multi-window sliding.  It is necessary to determine the length and
annotations of the window before training the classifier.  Therefore, annotations have a great
impact on fall detection performance.  Particularly, the annotation method used for labeling
sliding windows greatly affects sliding window-based fall detection.
## 3.3.2 Peak Detection Windowing
Human  gait  contains  peak  acceleration  and  angular  velocity  signals  when  fall  and
near-fall activities occur.  Hence,  the need to accurately detect the fall of senior subjects
requires us to differentiate the peak signal of near-fall from fall events.  Due to the above rea-
sons for the alternation in the signal values, we take the highest magnitudes of acceleration
as the basis to define the observation window of the detector.
Therefore, to use these advantages, we start the sliding window features engineering
from the maximum magnitude of the acceleration signal (as shown in the algorithm 1) rather
than using the conventional engineering of sliding window features.  This kind of windowing
helps  us  to  include  the  important  peak  signal  value  in  a  single  window,  besides  paying
great  attention  to  the  maximum  value  signal.   If  we  were  using  conventional  windowing
based on the raw signal, the important information might be shared in different windows.
For  our  proposed  method,  the  maximum  values  of  the  acceleration  and  angular  velocity

## 28
simulated in the accelerometer and gyroscope data sets, consisting of 3D acceleration data
A(t) = [A
x
(t),A
y
(t),A
z
(t)] and 3D angular velocityG(t) = [G
x
(t),G
y
(t),G
z
## (t)] [93], [94],
are calculated using the magnitude of these vectors for thei−thsample as follow:
## ∥A
i
## (t)∥=
q
## A
## 2
xi
(t) +A
## 2
yi
(t) +A
## 2
zi
## (t).(2)
∥Gi(t)∥=
q
## G
## 2
xi
(t) +G
## 2
yi
(t) +G
## 2
zi
## (t).(3)
Next, the maximum magnitude of the signal calculated above is determined as fol-
lows.
## ∥A
max
∥=max({∥A
i
∥:i∈[1 :M]}),(4)
## ∥G
max
∥=max({∥G
i
∥:i∈[1 :M]}),(5)
where  M  is  the  number  of  samples.   Using  the  above  two  equations  and  the  size  of  the
window (which is 0.2 seconds in our case), the sliding window around the peak signal looks
as shown in Figure 4.
After estimating the maximum acceleration and deciding on the size of the sliding
window, the algorithm above gives us the input features and their labels, which are formed
by simply concatenating the six features of the accelerometer and gyroscope,
## {A
xj
## ,A
yj
## ,A
zj
## ,G
xj
## ,G
yj
## ,G
zj
## |j∈[r
o
## −
w
## 2
f
s
## ,r
o
## +
w
## 2
f
s
]}, wherewis the duration of the obser-
vation window (0.2s) andf
s
the sampling rate of the sensor (100 Hz) [93], [94].  Finally, the
size of the input features (N
i
) that depends on the duration of the observation window (w)

## 29
isN
i
## = 6∗(w∗f
s
## + 1).
Algorithm 1Calculating the maximum value and labeling the window
input : A(t) = [A
x
(t),A
y
(t),A
z
(t)] andG(t) = [G
x
(t),G
y
(t),G
z
## (t)]
output:Observation windows and its label
forall data sequence activities of FallDatado
Calculate the magnitude using equations 2 and 3;
Calculate the maximum value using equations 4 and 5;
ifFallData is non-fallthen
Label all the observation windows as non-fall
else
ifthe observation window is maximum value windowthen
label the window as fall
else
Label ADL
end
end
end
## Peak Values
Fig. 4.  Sliding window based on the maximum signal (peak values).  We use the peak signal
values calculated as the basis to define the starting point of the observation window.
## 3.3.3 Multi-window Sliding
Contrary to the above two methods, we propose another Multi-window segmentation

## 30
approach that uses a dynamic main window based on the label of the datasets and sliding
conventional subwindows inside the main window.  A fixed-size overlapping sliding window
for the sub-windows inside the label-based main window is utilized.  Using the main window
based on the label of the datasets and the window size ( 0.2 seconds) of the sub-windows,
the sliding window approach looks like in algorithm 2.  The observation window is extracted
as shown in Figure 5.
Algorithm 2Sliding Multi-window
input : A(t) = [A
x
(t),A
y
(t),A
z
(t)] andG(t) = [G
x
(t),G
y
(t),G
z
## (t)]
output:Observation windows and its label
forall data sequence activities of fall datasetsdo
ifLabel is non-fallthen
Sliding sub-window from the starting to the end of non-fall label index
Label all the sub-window observation windows as non-fall
else ifLabel is Fallthen
Sliding sub-window from the starting to the end of Fall label index
Label all the sub-window observation windows as Fall
end
3.4 Compensating an Imbalanced Dataset
Recently,  several  researchers  have  been  developing  wearable  sensors  based  on  fall
detection.  However, those researchers neglect to consider that fall events are rare to occur
than ADLs; the fall datasets are imbalanced.  This section will discuss different methods of
balancing the datasets that led to high performance in deep learning-based fall detection.
The SDSU dataset is comprised of 13% of fall labels and 87% of non-fall activities.
The main problem of not considering such imbalanced datasets is our deep learning models
make our minor label classes suffer from low results, although the accuracy of those minority
classes is the most important one [95].  This dissertation presents different dataset balancing

## 31
Sliding Sub-Windows
Fall Large WindowNon-Fall Large Window
Fig. 5.  Illustration of Multi-window Sliding.  It uses a dynamic main window based on the
classes of the fall datasets (master windows: Non-fall Large window and Fall Large Window)
and sliding conventional subwindows inside these main windows.
methods:  re-sampling techniques, data augmentation, and customized loss function.  These
three  methods  are  compared  to  balance  our  training  datasets  so  that  the  deep  learning
methods will create results that will not suffer in the minor classes.
## 3.4.1 Re-sampling Techniques
The main objective of dataset balancing is to increase the incidence of the minority
class or decrease the incidence of the majority class.  This is done to obtain an equivalent fre-
quency of instances for both classes.  The most familiar resampling approaches for resolving
the imbalanced datasets problem are oversampling the minority class and undersampling
the majority class [95]–[97].  The best-known undersampling method is calledrandom  un-
dersampling, and it randomly deletes window signals from the major classes of the training

## 32
dataset.   The  most  common  and  simplest  method  of  this  oversampling  of  the  minority
class israndom  oversampling- which duplicates random sequences of window signals from
the minority class in the training datasets [95]–[97].  Undersampling for the majority class
loses  some  of  the  information,  whereas  oversampling  for  the  minority  class  does  not  lose
any data.  Undersampling helps to improve run-time and storage problems by reducing the
number of training data samples, even though it loses some of the information.  In contrast,
oversampling for the minority class does not lose any data, but it increases the likelihood
of  overfitting  since  it  replicates  the  minority  class  events.   As  a  result,  these  duplicated
examples do not provide new information to the model.
Due to the above reason, another method called Synthetic Minority Oversampling
Technique (SMOTE) [98] with random undersampling and random oversampling is used.
This  approach  of  balancing  datasets  helps  to  avoid  overfitting,  which  occurs  when  exact
replicas of minority instances are added to the main data set.  A subset of data is taken
from the minority class as an example, and then new similar synthetic instances are created.
These synthetic instances are then added to the original datasets.  The new data set is used
as a sample to train the classification models.  SMOTE works by selecting random examples
from the minority class that is close to the feature space, as shown in Figure 6. Subsequently,
its k nearest minority class neighbors are found, and then the line segment in the feature
space is formed to obtain a synthetic example generated randomly in this line segment [98].
## 3.4.2 Data Augmentation
In data augmentation, prior knowledge about the data’s invariant properties is in-
jected into various transformations.  A deep learning model can be enhanced by augmenting

## 33
a
b
c
Fig. 6.  Illustration of the SMOTE oversampling approach.
the input data to cover the unexplored input space, prevent overfitting, and improve general-
ization ability [6].  It is well known in image recognition that minor changes due to jittering,
scaling, cropping, warping, or rotating does not alter the data labels [99].  Although label-
preserving transformations can be applied to wearable sensor data, they are not intuitively
obvious.
Rotation:Data  from  wearable  sensors  are  subjected  to  rotational  data  augmen-
tation to introduce label-invariant variability [100].  The readings can be inverted without
changing the labels when the sensor is placed upside down.  As a result, 180-degree rotations
can be used to simulate upside-down sensor placements by augmenting existing data.  Based
on the acceleration vector [A](t) = [a
x
## (t),a
y
## (t),a
z
(t)] for time t, which contains acceleration
components along the x, y, and z axes, respectively, a new vector [A]∗r(t) can be obtained
by rotating [A](t) 180 degrees in the x, y, and z axes.
MixUp:MixUp augmentation is a technique that combines two examples of data to
enhance them in a more meaningful way [101].  A temporal MixUp is a method in which all

## 34
values of a set of features in the first input sequence (observation window) of the sensor are
multiplied by a random value,m, and then added, with all features of the same sensor from
a second randomly selected input sequence (observation window) of the sensor multiplied
bym.  Generally,mis chosen between the min and max of the observation window features
and is selected at random between the min and max values.  As a result of this operation,
all six features of the accelerometer and gyroscope are affected.  The overall operation of
Mixup augmentation is shown in Figure 7.
CutMix:By using the CutMix technique, two data samples can be combined [102].
A  temporal  CutMix  is  a  method  that  selects  a  random  time  segment  from  a  first  input
sequence (observation window) and a random time segment from a random second input
sequence (observation window) of both labels based on time segment selection.  As shown
in Figure 7,  both sequences have two random segments that start at the same time step
and end at the same time step.  Then, it selects a random set of sensors and replaces the
channel values of the first multi-feature segment with the channel values of the second multi-
feature segment.  Whenever the size of the random time segment is determined at random,
it ranges between the maximum and minimum values of the hyperparameters.  It turns out
that channel probability is a hyperparameter representing the chance each channel will be
selected for this operation on a given day.
## 3.4.3 Customized Loss Function
Lastly,  a customized loss function can also be applied to handling the imbalanced
classes.  To train the fall dataset, a customized focal loss presented by Lin et al.  [103] is used
for object detection.  The modified loss function is used to tune the model parameters while

## 35
## 1 2 3 4 5 1 2 3
## 4 3 4 3 4 3 4 3
## 1 2 2 1 1 1 2 2
## 5 4 2 1 1 2 4 5
## 1 2 2 1 1 1 2 3
## 4 3 4 3 4 3 4 3
## 1 2 2 1 1 1 2 2
## 5 4 2 1 1 2 4 5
## 1 2 3 4 5 1 2 3
## 4 3 4 3 4 3 4 3
## 1 2 2 1 1 1 2 2
## 6 5 2 1 2 3 6 5
## 1 2 2 1 1 1 2 3
## 5 4 3 2 3 3 5 4
## 1 2 2 1 1 1 2 2
## 5 4 2 1 1 2 4 5
CutMix
MixUp
Fig. 7.  Sample of CutMix and Mixup Augmentation.  Input sequence targets are in red, and
how they changed in blue.  A green value indicates a reference from another input sequence
used in the augmentation calculation.
training the fall dataset.  Even when wrongly classified samples are penalized more than the
correct ones, the loss function is overwhelmed with non-fall activity (ADL) classes in the fall
detection settings due to the imbalanced sample size.  Customized focal loss addresses this
problem and is designed to reduce the loss for the ADL classes; thus, the network can focus
on training the fall classes.  During supervised training of the dataset, the proposed deep
learning model is optimized end-to-end using a customized weighted focal loss in Equation
## 6:
## L
FocalLoss
## =
c=2
## X
i=1
w
i
## (1−p
i
## )
γ
log(p
i
## ),(6)
where
w
i
## =
n
## 0
## +n
## 1
## 2∗n
i
andn
## 0
is number of non-fall class,n
## 1
is number of fall class, andγis a focusing parameter
whose value isγ >= 0.  This focusing parameter specifies to reduce the influence of higher-
confidence classified samples of ADL classes in the loss.  The higher theγ, the higher the

## 36
rate at which easy-to-classify examples are down-weighted.  Ifγ= 0, a weighted focal loss
is equivalent to a weighted binary cross-entropy loss.
3.5 Sensor Positions and Sensor Types
Many fall detection studies [104] utilize an accelerometer as the primary sensor to
determine falls.  Chernbumroong [104] evaluates the importance of different sensors in a mul-
tisensory scheme, concluding that the most relevant features are derived from two particular
acceleration components (Z and Y). A gyroscope is considered computationally expensive
and is less useful in improving our understanding of the dynamics of falling [45], [51], [105].
In  contrast  to  this,  Nguyen  et  al.   [106]  report  that  a  sensing  module  (including  a  gyro-
scope and an accelerometer) consumes only about 3% more power when both sensors are
active (compared to only measuring the acceleration magnitudes when only one sensor is
activated).
Furthermore,  what type of sensors to use depends on the type of dataset;  such as
whether it contains near-fall activities or not.  Using only acceleration measurements can
result in many false positives and false negatives caused by near-fall activities, such as sitting
down quickly on a mattress.  Near-fall and fall activities have almost the same vertical signal
variation, making it difficult to differentiate. The gyroscope’s angular velocity measurements
can  significantly  reduce  false  positives  and  negatives  caused  by  near-fall  activities.   This
dissertation compares an accelerometer-based algorithm and a gyroscope-based algorithm
intended for fall detection.
The SDSU dataset is comprised of sensor acquisitions from sixteen different anatom-
ical locations:  head, pelvis, upper and lower thoracic, upper (2) and lower arms (2), hand

## 37
(2), thigh (2), shank (2), and foot (2).  Most studies in the literature on fall recognition have
been conducted using a single wearable device without taking into account the device’s lo-
cation on the user’s body.  The position information provided by the wearable device can,
however,  assist  in  improving  the  performance  of  the  fall  detection  system.   Several  body
locations are compared in this study to determine the most optimal location of the sensor’s
position for fall detection systems.

## 38
## CHAPTER 4
## IMAGE BASED TRANSFER LEARNING
This chapter presents work on fall detection using a 2D transfer learning method.
Inspired by the tremendous progress in image-based object recognition with convolutional
neural networks (CNNs),  we opt for a pre-trained kinematics-based machine learning ap-
proach through existing large-scale annotated accelerometry datasets.  The accelerometry
datasets  are  converted  to  2D  images  using  various  time  series  to  image  encoding  meth-
ods.  Subsequently, data augmentation is performed on these images to increase accuracy,
thereby complementing limited labeled fall sensor data and enabling transfer learning from
an existing pre-trained model.
## 4.1 Background
Convolutional neural networks, characterized by multiple intermediate “hidden” lay-
ers, have gained popularity due to their ability to recognize complex patterns in images and
videos for classification while providing highly nonlinear parameter-space boundaries that
enable sophisticated decisions [107]–[109].  However, there has been a paucity of formal un-
derstanding on how to interpret those patterns, why they perform well, and how they might
be optimized.  Researchers typically used trial and error by tuning variables to boost their
performance.  As a CNN is not a black box, we can visualize features learned to understand
the intermediate representation of the transformed data in the network.  After applying the
forward CNN for the images, these intermediate forms are themselves images and can be

## 39
visualized as images.  Recently, the ”Deconvolution approach” [110] was proposed to visu-
alize features on the hidden layers by projecting those feature activations back to the input
pixel space (images).  In the initial layers, low-level image features that are generic to any
datasets such as edges and curves are extracted, while in the deeper layers, more complex
features that are specific to the inputs are captured.  Hence, we can transfer the knowledge
learned from the initial layers network of existing large datasets to other datasets by taking
the generic lower-level features to any datasets (like blobs and edges).
This work describes a fall detection algorithm using transfer learning [24], [111], [112]
and transforming the tri-axial accelerometer fall sensor data into images to classify fall and
non-fall datasets through a pretrained CNN architecture.  The different encoding techniques
that represent the sensor data are used to create RGB images to be compatible with various
CNN architectures, which also facilitates visualization, where each RGB image input should
be an array of 227 by 227 by 3.  Data augmentation on these images is performed to increase
the size of the dataset used to fine-tune several deep CNNs.
Deep neural networks (like CNNs and RNNs) have recently been employed for time
series classification of human activity recognition tasks.  To train deep neural networks like
AlexNet from scratch, we would need a large dataset like the University of California Irvine
(UCL)  human  activities  dataset  [46].   However,  it  is  difficult  to  get  enough  fall  datasets,
let alone fall datasets of elderly subjects,  for training deep neural networks from scratch.
Hence,  it  is  known  that  a  model  pre-trained  from  generic  features  extracted  and  learned
from large, existing, unrelated image datasets may confer advantages in applications with
fewer data, a technique known as transfer learning [113].  To deal with this limitation, we

## 40
propose applying transfer learning (TL) from deep CNN methods to classify our wearable
sensor  data,  which  helps  to  cope  with  otherwise  insufficient  data.   We  also  apply  several
image transformation methods to convert the sensor data to RGB images, suitable for the
input of our CNN architectures.
A publicly available University of Rzeszow fall detection dataset URFD [44] has been
used as input to our transfer learning-based CNN. This dataset contains 70 sequences, 30
of which involve falls, while 40 of these 70 sequences consist of non-fall activities.  Kepski
et al.  recorded these datasets from two accelerometry placed at the waist, in addition to
depth images that were collected using two Microsoft Kinect cameras.  For our proposed
method, we use the accelerometer data only, which consists of 3D acceleration vectorsA(t) =
## [A
x
(t),A
y
(t),A
z
(t)] and also the magnitude of these 3D acceleration vectors is computed as
follows:
## ∥A
i
## (t)∥=
q
## A
## 2
x
(t) +A
## 2
y
(t) +A
## 2
z
## (t)(7)
Due  to  the  beneficial  impact  of  additional  training  data  on  model  generalization
and performance [100], a data augmentation procedure involving random rotations has also
been used.  Moreover, data augmentation introduces a label-invariant extension of wearable
sensor  data  [100].   For  example,  an  upside-down  sensor  placement  can  invert  the  sensor
readings’ sign without changing the labels.  Therefore, augmentation by applying arbitrary
rotations to the existing data can be used to simulate different sensor placements.  Given
the acceleration vectorA(t) at time t, a new vectorA
r
(t) can be obtained by rotatingA(t)
by 10 degrees in the x, y, and z-axis is performed.

## 41
4.2 Wearable Sensor Signal Transforming into Images
## 4.2.1 Continuous Wavelet Transform
Ravi  et  al.   [9]  show  the  importance  of  using  a  suitable  domain  when  applying  a
deep learning methodology to time-series data.  Specifically, they show that the scalogram is
essential for extracting interpretable features that capture the differences among the nearest
inertial sensor data points.  The scalogram representation also provides a form of time and
sampling-rate  invariance.   This  enables  the  classification  to  be  more  robust  against  data
shifting  in  time  and  against  changes  in  signal  amplitude  and  sampling  rate.   Moreover,
frequency selection in the scalogram domain also provides an implicit way to allow noise
filtering of the data over time [114].
Mathematically, CWT represents time-varying spectral information of a continuous
signal  by  computing  the  inner  products  of  this  signal  with  a  set  of  continuous  wavelets.
To create time-frequency representations of the accelerometer signals of the wearable sensor
data, we have to pre-compute a CWT filter bank.  Pre-computing the CWT filter bank is the
most computationally efficient method when obtaining the CWT of many signals using the
same parameters.  These representations are aggregated into a scalogram, which summarizes
the magnitude of the CWT coefficients of a signal.  Our inputs to the CNN architectures are
the scalograms RGB images as shown in Figure 8.  To be compatible with CNN, each RGB
image is transformed into an array of 224-by-224-by-3.  The continuous wavelet transform
is defined as follows:

## 42
## CWT
ψ
x
## (τ,s) = Ψ
ψ
x
## (τ,s) =
## 1
p
## |s|
## Z
x(t)ψ
## ∗
## 
t−τ
s
## 
dt(8)
The inner product of two functions is defined by:
< f(t),g(t)>=
## Z
b
a
f(t)·g
## ∗
## (t)dt(9)
According to the above definition of the inner product, the CWT can be thought of
as the inner product of the test signal with the basis functionsψ
## (
τ,s)(t) :
## CWT
ψ
x
## (τ,s) = Ψ
ψ
x
## (τ,s) =
## Z
x(t)·ψ
## ∗
τ,s
## (t)dt(10)
where,
ψ
τ,s
## =
## 1
## √
s
ψ
##  
t−τ
s
## 
## Fig. 8.  Sample Scalogram.

## 43
To apply transfer learning to our fall datasets, we must transform the 1D signal data
to  RGB  images  (three-channel  input).   This  conversion  is  necessary  because  our  transfer
learning approach exploits the feature vectors from the pre-trained model of the ImageNet
datasets  [14],  and  various  architectures  are  designed  to  classify  images  (three-channel  in-
puts).   We  can  apply  these  CNNs  network  architecture  to  classify  accelerometer  signals
based on scalogram images computed with the time series data’s continuous wavelet trans-
form (CWT).
## 4.2.2 Gramian Angular Field
A Gramian Angular Field (GAF) is a mathematical tool used to obtain images from
time series data [115].  As a first step, polar coordinates are used to represent a time series.
After  computing  the  sum  of  the  angular  coordinates,  the  time  series  can  be  transformed
into a Gramian Angular Summation Field (GASF). It converts a time series into an image
by computing the pairwise dot products between all pairs of time points and then applying
a  nonlinear  transformation  to  map  the  resulting  Gramian  matrix  to  a  grayscale  image.
Specifically,  the GAF is defined as follows:  Given a time seriesA
x
## =a
x1
## ,a
x2
## ,...,a
xn
## ,the
Gramian matrix G is defined as follows:
## G
ij
## =a
xi
## ∗a
xj
## +a
xj
## ∗a
xi
## (11)
The elements of the Gramian matrix are then normalized to lie in the range [0,1] by
dividing each element by the maximum element in the matrix.  Finally, a nonlinear mapping
function is applied to the elements of the scaled Gramian matrix to map them to a grayscale

## 44
image.  Considering our time series is composed of N timestamps t with corresponding values
x, the angles are computed using arccos(x).  They lie within [0,π] and divide the interval [0,
1] into N equal parts to calculate the radius variable.  This results in N+1 delimiting points
(0, ..., 1).  In the next step, discard 0 and add these points consecutively to the time series.
Mathematically the 2D Encoding of the scaled time series translates as follows:
## 
## 
## 
## 
## 
## 
## 
## 
## 
φ
i
arccos(x
i
## )
r
i
i/N
## (12)
The GAF has been shown to effectively represent sequential data by converting time
series into images [115], [116].  Additionally, the GAF can capture both the temporal and
spatial correlations in time series data.
Fig. 9.  Sample Image Transformed using Gramian Angular Field.

## 45
## 4.2.3 Markov Transition Field
Markov Transition Field (MTF) is another mathematical tool used to encode time
series data into images [117].  The MTF converts a time series into a matrix by computing
the transition probabilities between all pairs of adjacent time points and then mapping the
resulting matrix to an image using a grayscale color map.  Specifically, the MTF is defined
as follows:  Given a time seriesA
x
## =a
x1
## ,a
x2
## ,...,a
xn
,the transition probability matrix P is
defined as follows:
## P
ij
=Count(a
xi
## ,a
xj
)/Count(a
xi
## )(13)
whereCount(a
xi
## ,a
xj
) is the number of timesa
xi
is followed bya
xj
in the time series,
andCount(a
xi
) is the total number of timesa
xi
appears in the time series.
The elements of the transition probability matrix are then mapped to a grayscale
image using a linear or nonlinear mapping function.  The inverse hyperbolic sine (arcsinh)
function is the most commonly used mapping function [117].  Additionally, the MTF can
capture the statistical properties of the time series, such as its stationarity and autocorre-
lation structure, which can be useful in various applications such as anomaly detection and
prediction.
## 4.2.4 Recurrence Plot
A  recurrence  plot  (RP)  is  a  visualization  of  a  square  matrix  in  which  the  matrix
elements correspond to those times at which a state of a dynamical system recurs (columns

## 46
Fig. 10.  Sample Image Transformed using Markov Transition Field Image.
and rows correspond then to a certain pair of times) [118], [119].  Technically, the recurrence
plot reveals all the times when the phase space trajectory of the dynamical system visits
roughly  the  same  area  in  the  phase  space.   It  is  a  matrix  obtained  from  a  time  series,
representing the pairwise Euclidean distances for each value (and more generally for each
trajectory) in the time series.  The plot can be binarized using a threshold [118], [119].
Recurrence plots describe non-stationarity and can also detect a dynamical system’s
periodicity [118].  It takes a point in timet
## 1
and asks what are the other points in time (t
i
## )
when the object has gotten very close to where it was at the timet
## 1
, so in some sense, this
captures strict patterns in time:  when the value in the future is very close value now, the
corresponding pixel is black, else it is white.
A recurrence plot represents the distances between trajectories extracted from the
original time series.  This recurrence plot extracts trajectories from time series and computes

## 47
the pairwise distances between these trajectories.  Some of the methods used for measuring
the distance in phase space are Manhattan and Euclidean norms, as shown in the equations
below.
ManhattanDistance=

n
## X
i=1
## |x
i
## −y
i
## |
p
## !
## 1/p
## (14)
## Eucleadian
## Distance=
v
u
u
t
n
## X
i=1
## (x
i
## −y
i
## )
## 2
## )(15)
Maximum diagonal  line  length is  applied  for calculating  commonly used  measures
of recurrence quantification analysis (RQA). The trajectories are defined as follows [118],
## [119]:
Where m is the dimension of the trajectories andτis the time delay.  The recurrence
plot, denoted R, is the binarized pairwise distance matrix between the trajectories:
In a given time series, it has a 2D squared array matrix (just one channel).  If we
apply a color map to the array, then we’ll get a 3D array with three channels.  But we don’t
necessarily need to do that.  If one wants to apply three recurrence plots to 3-time series
(of the same length), we can create them.  And we would get an image with three channels.
Subsequently, the model is trained based on the intended input size.  If we want to represent
the recurrence plot with more than 3D arrays that take our neural network more than three

## 48
channels, one would then need to modify our neural network to have as many channels as
needed [118], [119].
Fig. 11.  Sample Image Transformed using Recurrence Plot.
4.3 Transfer Learning with Deep CNN
In practice, we don’t usually train an entire CNN from scratch with random initial-
ization.   Indeed,  it  is  relatively  rare  to  have  a  sufficiently  large  dataset  for  the  depth  of
network required.  Instead, it is recommended to pre-train a CNN on a very large dataset
and then use the trained CNN weights either as an initialization or a fixed feature extractor
for  the  task  of  interest,  which  we  call  transfer  learning.   The  low  and  high-level  features
learned by a CNN on a source domain can often be transferred to augment the learning in
a different target domain.  For target problems with a large dataset,  we can transfer the

## 49
low-level features (such as edges and corners), and learn new high-level features specific to
the target problem.  Learning new high-level features is difficult for target problems with
limited  data.   However,  if  the  source  and  target  domains  are  sufficiently  similar,  the  fea-
ture representation learned by the CNN on the source task can be transferred to the target
problem.  Deep features extracted from CNNs trained on large annotated datasets of images
have been used as generic features very effectively for a wide range of computer vision tasks
## [107], [120].
A CNN is difficult to apply in the medical field since collecting sufficiently extensive
data from patients is challenging.  However, transfer learning can be applied in cases with
limited training data.  For image recognition, the early layers of the network typically learn
low-level features such as edges and corners, and further layers learn high-level features such
as textures and objects [110].  Hence, the pre-trained initial layers can be used to extract
low-level features, and the last few layers can be retrained and tailored to extract specific
features related to our limited fall dataset.
Three different state-of-the-art CNN architectures were investigated:  AlexNet [70],
ResNet50 [121], and InceptionV3 [69].  They were all pre-trained on the ImageNet dataset
[14].  The overall model for image-based transfer learning can be shown in Figure 12.
4.3.1 AlexNet
AlexNet deep CNN contains five convolutional layers, three fully connected layers,
and dropout applied before the first and second fully connected layers. The network achieved
state-of-the-art performance (15.3% VS 26.2% for the second place) in the 2012 ImageNet
LSVRC-2012 competition.

## 50
## Fine-
## Tuning
AlexNet
ResNet50
InceptionV3
## Pre-trained Models
## Transfer Learning
## Fall Detection
## Images
## Fall
## Non-fall
## Raw Signals
Fig. 12.  Overall Model of Image-based Transfer Learning.
AlexNet is a deep CNN whose architecture supports images of 227 by 227 with three
channels  (RGB),  as  shown  in  Figure  13.   The  RGB  images  of  accelerometry  data  of  the
## ∥A
i
(t)∥are  the  input  to  our  architecture.   Fine-tuning  the  transferred  model  allows  us
to bring a significant improvement of average accuracy for our fall detection classification
with  insufficient  data.   We  fine-tune  the  weights  of  the  pre-trained  CNN  of  ImageNet  by
continuing the back-propagation.  It is possible to fine-tune all the layers of the CNN, or
alternately to keep some of the earlier layers fixed (due to overfitting concerns) and only fine-
tune some higher-level portions of the network.  This transfer learning strategy is motivated
by the observation that the earlier features of a CNN contain more generic features (e.g.,
edge detectors or color blob detectors) that should be useful to many tasks, but later layers
of the CNN become progressively more specific to the details of the classes contained in the
fall  image  dataset.   Our  fall  detection  dataset  is  small,  so  we  can  train  a  linear  classifier
for the feature vectors learned from the ImageNet dataset.  However,  the RGB images of
our fall datasets are very different from the ImageNet dataset, so it is better to train the
architecture with our fall datasets earlier in the network than applying a linear classifier at
the end layer of the network containing more input-specific features of ImageNet [24], [111],

## 51
## [112].
Consequently,  the last three layers of the AlexNet architecture are altered and re-
trained for classifying the scalogram images of our fall dataset.  Those three layers must be
fine-tuned for our fall classification problem:  the fully connected layer, which is configured
for 1000 categories by default, must be set to the same size as the number of categories in
our fall dataset, which is two classes in our case.  The Softmax classifier [122], which provides
a probability for each class in a multi-class problem, is also replaced with the support vector
machine(SVM) classifier that classifies fall and non-fall events in a straightforward manner
[112].   Furthermore,  the  SVM  classifier  provides  stable  results  and  trains  faster  than  the
Softmax classifier.
Fig. 13.  Proposed AlexNet Architecture.
4.3.2 ResNet50
ResNet50 has 50 layers and uses skip connections to pass information forward through
the network- 49 convolutional layers, and one fully connected layer [121].  The architecture

## 52
of ResNet50 is based on a series of residual blocks.  Each residual block consists of two or
more convolutional layers, with a skip connection that adds the output of the convolutional
layers to the input of the block.  This allows the network to learn residual mappings, which
can be used to improve the accuracy of the network [121].
It  uses  residual  mapping,  which  aids  in  avoiding  degradation  problems  that  occur
with  very  deep  CNNs.   In  addition,  the  intermediate  normalization  layers  of  the  ResNet
also solves the problem of vanishing and exploding gradients.  These skip connections allow
gradients to flow more easily during training, which helps to prevent the vanishing gradient
problem that can occur in deep neural networks [121].
4.3.3 InceptionV3
The  Inception  network,  used  in  this  study  and  introduced  by  Szegedy  [69],  which
had 95 layers.  It is part of the Inception family of models, which are designed to optimize
the tradeoff between computational efficiency and accuracy in large-scale image recognition
tasks [69].  InceptionV3 uses a combination of convolutional layers with different filter sizes,
pooling layers, and Inception modules, which are composed of parallel convolutional layers
with different filter sizes and pooling layers.  The architecture also includes batch normal-
ization layers,  dropout layers,  and global average pooling layers [69].  The architecture is
designed to overcome the difficulty of choosing the filter size or when to use the pooling layer.
InceptionV3 solves this by supplying different filter sizes and pooling layers in parallel.
One key innovation in InceptionV3 is the use of ”factorization” methods to reduce
the number of parameters in the network. This includes using 1x1 convolutions to reduce the
number of channels in feature maps before applying larger convolutions, and using separable

## 53
convolutions, which split the convolution operation into separate spatial and channel-wise
convolutions.  Computational complexity is reduced by using a one×one convolution to
shrink the volume of the next layer [69].
## 4.4 Summary
This chapter described a fall detection methodology that combines transfer learning
and  image-encoded  tri-axial  accelerometer  sensor-based  data  to  classify  fall  and  non-fall
data sets in conjunction with pre-trained CNNs based on Deep CNN. The TL method was
used to overcome the difficulties of training a deep CNN resulting from limited fall datasets
by transferring knowledge learned from many existing ImageNet datasets [14].
To develop real time fall detection system using the image-based model, the CNN
model should be trained on a GPU processor and then convert the trained model to Ten-
sorFlow Lite [123].  TensorFlow Lite provides a method that converts the generated model
into a TensorFlow Lite FlatBuffer format file (.tflite), which can be deployed on an FPGA.
FPGAs  provide  low  latency.   This  enables  real-time  applications  for  our  image-based  fall
detection system.

## 54
## CHAPTER 5
## DEEP LEARNING MODELS USING RAW SENSOR SIGNALS
## 5.1 Background
Over  the  last  decade,  deep  neural  network  methods  have  had  the  ability  to  learn
complex  patterns  in  images  and  sensor  data,  and  they  are  achieving  many  state-of-the-
art  results  in  different  applications.   Inspired  by  the  success  of  CNN  [124],  [125],  LSTM
[13], and Transformer [12] in related applications, this chapter explores deep learning-based
fall  detection  systems  using  raw  sensor  signals.   Various  deep  learning  models  are  used  -
fully connected networks (FCN), long short-term memory- fully connected network (LSTM-
FCN), a residual neural network (ResNet), an attention neural network, and convolution-
based feature extractor with LSTM and Transformer encoders.  Furthermore, self-supervised
learning that pre-trains unlabeled data and fine-tunes the network using small labeled data
is developed.
## 5.2 One-dimensional Convolutional Neural Networks
In  this  section,  we  develop  a  one-dimensional  convolutional  neural  network  model
(1D CNN) fall detection based on raw sensor signals.  Convolutional neural network models
were developed for image classification problems [24].  The model learns from an internal
representation of a two-dimensional input in a process called feature learning.  This same
process can be applied for fall detection in the one-dimensional raw time series accelerometry

## 55
and gyroscope data.  The benefit of using 1D CNNs for sequence classification is that they
can directly learn from the raw time series data.  They do not require domain expertise to
manually engineer input features.  The model can learn from an internal representation of
time series data.
Three 1D CNN architectures are used for training and testing the wearable sensor
signals  -  Fully  Convolutional  Neural  Networks  (FCNs),  Long  Short  Term  Memory-FCN
(LSTM-FCN), and Deep Residual Networks (ResNet).  The SDSU fall datasets are used for
evaluating the performance metrics of those models [71], [83].
5.2.1 Fully Convolutional Networks (FCN)
Temporal convolutions have proven to be an effective learning model for sensor signal
sequence data [125].  Fully Convolutional Networks comprised of temporal convolutions are
like CNN; they are mainly used for feature extraction.  Fully Convolutional Neural Networks
(FCNs) are similar to the standard convolutional neural networks (CNN) without the local
pooling layers - which means the input size of the sensor signal will not change in size when
it goes further into deeper into hidden layers [125].  Moreover, the main difference between
CNN and FCN models is that the last fully connected layer is replaced by global average
pooling in FCNs.  One of FCN architecture’s main characteristics is replacing the traditional
final FC layer.  Hence, avoiding the fully connected layer helps us decrease the number of
parameters learned in the neural network.  Our model adapted from [125] as shown in Fig-
ure 14 consists of three stacked convolution layers of the block where a single block has a
convolution, batch normalization [91], and ReLU sub-layers.  As we can see from the archi-
tecture, there are no sub-sampling (pooling) and dropout (regularization) layers.  The three

## 56
convolution layers have 512 filters with a kernel size of 5, 256 with a kernel size of 5, and
128 filters with a kernel size of 3 consecutively.  To keep the sequence length of the input
signal after the convolution operation, each convolution layers use a stride length of 1 with
zero padding.  Each layer of the convolution is followed by batch normalization and then
passes through the ReLU activation function.  Global average pooling layer is applied to the
result found from the last convolution.  Finally,  a sigmoid classifier is used for classifying
falls from non-fall activity.
Fig. 14.  FCN model architecture with respective filter sizes followed by a Sigmoid layer.
5.2.2 Long Short Term Memory-Fully Convolution Network (LSTM-FCN)
The LSTM-FCN method first proposed by Karim [126] is the model created by aug-
menting the FCN model with an LSTM block.  The LSTM block contains LSTM operation
and  dropout  regularization  preceded  by  the  dimension  shuffle  layer.   In  our  proposed  ar-
chitecture, as shown in Figure 15, the FCN part has three stacked convolutional blocks -
where each of the blocks contains temporal convolution, batch normalization [91] and ReLU
operation.  The three convolution layers have 512 filters with kernel sizes of 5, 256 with a
kernel size of 5, and 128 filters with a kernel size of 3 consecutively.  To keep the sequence
length  of  the  input  signal  after  the  convolution  operation,  each  convolution  layers  use  a

## 57
stride  length  of  1  with  zero  padding.   Each  layer  of  the  convolution  is  followed  by  batch
normalization and then passes through the ReLU activation function.  The global average
pooling layer- which replaced the fully connected layer of CNN to decrease the number of
parameters  in  the  model,  is  applied  to  the  result  found  from  the  last  convolution  block.
Both  LSTM  and  temporal  convolution  blocks  accept  the  sensor  signal  input  of  the  same
dimension of the different formats [126].  The LSTM layer accepts a single step of all the
features, whereas the input signal format of the temporal convolutional block considers a
single feature with multiple steps at a time.  Here is inserted the dimensional shuffle layer, in
order to adjust the input dimension of both blocks by transposing the LSTM block.  Next,
the feature vector layers concatenate the output features of both blocks into one to be fed
to the sigmoid classifier.  Finally, a sigmoid classifier is used for classifying falls from non-fall
activity.
Fig. 15.  The LSTM-FCN architecture with respective filter sizes- It concatenates a standard
FCN (upper part) and LSTM (Lower part) and finishes with a Sigmoid layer.

## 58
5.2.3 Residual Network (ResNet)
The last proposed architecture based on the 1D CNN method is Deep Residual Net-
work  (ResNet).   The  ResNet  model  has  eleven  layers-  of  these  layers,  nine  of  them  are
convolution, followed by global average pooling that calculates the average of the input sen-
sor signal dimension as shown in Figure 16.  The main difference between ResNet and other
convolution-based deep learning models is that ResNet introduces a residual connection be-
tween successive convolutional layers.  In addition to that, the ResNet makes the network’s
training faster by decreasing the vanishing gradient problem.  ResNet achieves this efficient
training by using a linear shortcut between the successive skip connection that makes the
flow of the gradient directly through these residual connections [124].  As depicted in Figure
16, The ResNet model has three stacked residual blocks, each with three convolutions.  The
result of each residual block is added to the input of each residual block to be output to
the next layer.  The output of the last residual block is then followed by a global average
pooling layer and then a softmax classifier with two neurons as our number of classes are
two (fall and non-fall).  In all the residual blocks, the three convolutions have kernel lengths
of 8,8 and 5 with 64 number of filter for three of the convolution.  The convolution layers are
followed by batch normalization and ReLu operations.  All of the layers have an invariant
number of parameters, similarly to the above two models- LSTM-FCN and FCN.
## 5.3 Attention Neural Network
In this section,  we develop the second method that uses attention-based networks
for sensor signals to create a robust fall detection system.  Most fall detection systems use

## 59
Fig.  16.   ResNet  model  architecture  [125]  with  respective  filter  sizes  to  be  used  for  pre-
training and fine-tuning by replacing FCN in the framework above.
wearable  devices  with  inertial  or  accelerometer  sensors  as  the  main  data  source,  comple-
mented  with  other  sensors  like  gyroscopes  and  magnetometers  [41],  [127].   Adding  extra
parameters, such as angular velocity, helps enhance the algorithm performance but affects
the computational demand simultaneously.  Independently of the device used, many fall de-
tection methods have problems with false positives, false negatives, and computational time
[45], [63].  We aim to reduce false positives and negatives using IMU sensors coupled with
low-consumption  algorithms.   Our  work  uses  an  attention  neural  network  [12],  [26],  [31],
which has the advantage of enabling parallel computation and reduces the false positives
and false negatives by giving high attention scores for the observation windows that contain
important signal information.  As shown in Figure 17, the attention layer has an input called
query (q), which provides an output based on the memory, which contains a set of keys (k)
and values (v).  Self-Attention calculates the attention units using a scaled dot product and
learns by focusing attention to the previously generated data to derive new data [12].  Scaled
dot product attention is calculated for each unit within the input vector.  Query (W
q
), key

## 60
Multi-Head Attention
## V
Scaled Dot-Product Attention
## Linear
## KQ
## Linear
h
## Linear
## Linear
## Concat
## Q
## VK
MatMul
MatMul
## Scale
Mask (Opt.)
SoftMax
Scale-Dot Product Attention
Fig. 17.  Query, Keys and Values in Attention Mechanism.
## (W
k
), and value (W
v
) weights are calculated as:
Attention(Q,K,V) =Softmax(
## Q.K
## T
## √
d
k
## ).V(16)
where  the  query  is  an  entity  within  the  sequence,  keys  are  vector  representations  of  the
input, and the values are derived by querying against keys.  The term self-attention comes
from the fact that Query, Key, and Value are received from the same source, and generation
is unsupervised.

## 61
Attention neural networks have powered significant recent progress in natural lan-
guage processing.  They served as the foundation for architectures like Transformer [12], and
BERT [128] to form powerful language models that can be used for machine translation,
sentiment analysis, and detection documents [29], [34], [36]. With their recent success in nat-
ural language processing [128], [129], one would expect widespread adaptation to problems
like our fall detection datasets.  After all, both involve processing sequential data.  However,
to this point, research on their adaptation to sequential data problems has remained limited.
This section proposes a new approach that uses attention-based networks for sensor
signals to create a robust fall detection system. Cheng [130], in his paper on natural language
processing (NLP) using attention network,  highlighted his focus on certain words from a
given  sentence  when  reading  the  sentence.   It  is  not  difficult  for  the  reader  to  relate  a
given the word with the other words in the sentence, but a neural network would need to
specifically design parts of it to replicate this attention to certain words.  Similar to the NLP,
in our case, first, we form input features that feed into the attention network by finding the
maximum acceleration magnitude vector and the fixed window size around this peak signal
magnitude (Algorithm 1), which is then used as input to our attention networks.
Simply, our method aims to select useful information across the various features of
sequential data for predicting the time series data.  Human gait contains peak acceleration
and angular velocity signals when fall and near-fall activities occur.  Importantly, the need
to accurately detect the fall of senior subjects requires us to differentiate the peak signal
of  near-fall  from  fall  events.   Due  to  the  above  reasons  for  the  alternation  in  the  signal
values,  the  highest  signal  values  of  the  acceleration  are  taken  as  the  basis  to  define  the

## 62
observation  window  of  the  detector.   And  then,  high  attention  to  those  intervals  of  the
observation windows is given during the analysis of the fall detector network.  The attention
weight vectors select those variables that are helpful for fall detection and give them high
coefficient values.  The context vector is now the weighted sum of the observation window
vectors, which contains the information across multiple time steps.
Attention models are networks that introduce a weighting of signals based on relative
importance- which helps the model to emphasize important pieces in the feature space.  To
calculate the attention layer’s output, the input windows’ first score values are calculated
using the score function(α) bya
i
## =α(q,k
i
).  Next, the Softmax is used to find the attention
weights for each observation windowsb
i
## = [b
## 1
## ,b
## 2
## ,..,b
n
] computed byb
i
## =
exp(a
i
## )
## P
j
exp(a
j
## )
## .  Finally,
the output of the attention layer is the weighted sum of the values(o
i
## =
## P
n
i
## (b
i
## ∗v
i
## )) [131].
Hence, this weighted sum mechanism allows the model to focus and place more attention
on the most salient parts of the input sequence.
Therefore, to use the emphasis on salient points of the attention network by giving
the highest attention score to the determinant window of signal, this attention mechanism
proceeds  through  a  sliding  window  feature  determined  from  the  peak  magnitude  of  the
acceleration signal, based on the peak-window segmentation rather than using a conventional
sliding window input.  In addition, the peak signal of the gyroscope sensor is calculated for
creating a window on the peak of angular velocity, but the window size is decided based on
the accelerometer peak values instead of using its own peaks.  This kind of windowing helps
us to include the important peak signal values in a single window, in addition to giving high
attention to the maximum value signal.  If we were using conventional windowing based on

## 63
the raw signal, the important information might be shared in different windows.  For our
proposed method, the maximum values of the acceleration and angular velocity simulated in
the datasets of the accelerometer and gyroscope data, which consist of 3D acceleration data
A(t) = [A
x
(t),A
y
(t),A
z
(t)] and 3D angular velocityG(t) = [G
x
(t),G
y
(t),G
z
(t)], which are
calculated using peak-sliding window segmentation 1.
For the attention mechanism to function properly,  it needs to attach the meaning
of time to our input features.  The original NLP model added a collection of superimposed
sinusoidal  functions  to  each  input  embedding  [12].   We  require  a  different  representation
now that our inputs are scalar values, rather than distinct words (tokens).
Fig. 18.  Fall detection attention network.
After  the  input  features  to  our  attention  neural  network  model  are  selected,  the

## 64
sequence  of  time  (time-embedding)  is  encoded,  which  is  hidden  in  our  signal  data.   The
existing method Time2Vec [132] for time embedding is implemented.  This time embedding
is a vector representation just like a normal embedding layer that can be added to a neural
network architecture to improve a model’s performance and overcome an attention neural
network’s  temporal  indifference.   The  mathematical  representation  of  Time2Vec  for  the
ideas of periodic and non-periodic patterns, as well as its invariance to time re-scaling, are
presented by the mathematical on equation 17.
t2v(τ)[i] =
## 
## 
## 
## 
## 
## 
## 
ω
i
τ+φ
i
fori= 0
## (ω
i
τ+φ
i
)   for   1≤i≤k
## (17)
where  is a periodic function andφ
i
andω
i
are learnable parameters.
The network architecture used for our fall detection method is shown in Figure 18.
Since the decoder layers are not need for such an application, only the attention encoder lay-
ers similar to Google’s BERT network [128] network, with stacks of multi-head self-attention,
are used in this work [12].  Unlike BERT [128], which supports a 2D sequence input, our
model can process 3D sequential data- where the dimensions are sequence length, feature
sizeN
i
,  and batch size.  As shown in Figure 18,  the model that we propose incorporates
stacks of encoder layers contains positional encoding, three attention blocks, and two fully
connected layers.  The three identical attention block layers of our transformer encoder have
two  sublayers.   The  first  is  a  multi-head  self-attention  with  layer  normalization,  and  the
second is a feed-forward neural network.  In the self-attention encoder layer, the matrices
of queries, keys, and values are taken from the outputs of the previous encoder layer.  And
also, between these sublayers, there is a residual connection followed by layer normalization.

## 65
Next, we get a vector representation output from the transformer encoder model for each
input sequence length.  Finally, the last layer- The Sigmoid layer, receives the vector repre-
sentation output of the transformer encoder and provides us with a decision on whether it
is a fall class or a non-fall class.
5.4 Convolution Features with LSTM and Transformer Encoder
In this section, we propose another new approach that uses a convolutional feature
extractor  with  Encoder  (LSTM  or  Transformer)  for  sensor  signals  to  create  a  robust  fall
detection system.  As shown in Figure 19, the proposed overall architecture contains three
parts:  feature extractor, Encoder block (LSTM and Transformer), and linear classifier.  The
feature extractor part uses three convolutions with different filter sizes to extract features
from  the  segmented  input  sequence.   The  extracted  features  and  the  input  sensor  signal
are  concatenated  to  obtain  4N,  where  N  is  the  number  of  features  of  the  input  signals:
accelerometry and gyroscope signals.  In the Encoder block, we will use two different mod-
els to extract the dependency among the input sensor signals.  The first has three stacked
bidirectional LSTM encoders, and the second one has three stacked transformer encoders.
Each LSTM or Transformer encoder layer is followed by layer normalization and then passes
through the Gelu activation function [133].  The global average pooling and dense layers are
applied to the result computed by the last LSTM or Transformer encoder block.  Finally, a
Sigmoid classifier is used for classifying falls from non-fall activities.

## 66
Input Sensor Data (Ax,Ay,Az,Gz,Gy,Gz) (BSxSeqxN)
## Input Sequence
## LSTM/
## Transformer
## Encoder
## Block
## FC
## Sigmoid
## Fall
Non-Fall
Conv1D      k=4
Conv1D      k=3
Conv1D      k=5
## Feature Extractor
## Concatenate
## Global Pooling
LN + GeLU
BSxSeqx4N
BS-Batch size
Seq-Sequence Length
N-Feature size
k-kernel size
## Transformer Encoder Block
LSTM Block
## Overall Model Architecture
## Input
(BSxSeqx4N)
BiLSTM
## Norm
## Norm
## +
## +
## 3x
## Time Embedding
## Seqence
MH Attention
## Norm
## Norm
## +
## +
## 3x
Fig. 19.  Architecture of the Convolutional Feature Extractor with Encoder Block:  Trans-
former Encoder Block, and LSTM Encoder Block.
## 5.4.1 Feature Extractor
In a feature extractor block, kernels or filters are applied to project input features onto
another meaningful dimension to produce hidden feature maps.  Using hidden feature maps
as input for the following layers,  the networks can accurately extract or comprehend the
original input’s semantic meaning [109].  With a convolutional layer, the kernels (filters) scan
multiple cells at once, revealing hidden meanings and effects between them.  Convolutional
layers are often used because of their ability to extract adjacent cells and faster inference

## 67
times compared to RNNs.
Each  Conv1D  element  extracts  features  from  a  one-dimensional  acceleration  and
gyroscopic data sequence.  The model learns to extract features from sequences of observa-
tions and map the internal features to the Encoder Block.  In order to extract dense feature
representations of our input signals for effective learning, three different representations of
features with varying filter sizes are computed.  The convolutional layers are responsible for
processing the feature extraction tasks.  It processes the extraction by applying convolution
operations  to  the  input,  and  the  convolution  result  is  fed  to  the  next  layer’s  input.   The
convolutions operations using several filters or kernel sizes help us extract different input
features.
5.4.2 LSTM Encoder
Long  Short-Term  Memory  (LSTM)  has  been  considered  a  highly  successful  RNN
architecture  for  sequence  modeling.   When  time  is  a  relevant  feature,  the  easiest  way  to
handle it is to concatenate the time features with the input and use the LSTM model [27].
Due to their ability to learn how and when to forget and when not to use gates, LSTMs,
and their bidirectional variants are popular.
When  sequential  data  are  processed,  the  LSTM  often  ignores  future  information.
Based on LSTM, Bidirectional LSTM (BiLSTM) processes the data in the series forward
and  reverse,  connecting  the  two  hidden  layers  to  the  same  output  layer  [17]  and  storing
previous and subsequent information as the current time basis for the time series data [134],
[135].   Accordingly,  multi-directional  LSTM  is  more  accurate  than  unidirectional  LSTM.
In BiLSTM [134], the hidden layer output includes both forward and backward activation

## 68
outputs.
## 5.4.3 Transformer Encoder
In  this  section,  we  use  the  transformer  encoder  based  on  the  Vision  Transformer
(ViT) [16] that is developed for image recognition.  In a recent publication, Vaswani et al.
[12]  proposed  the  implementation  of  a  multi-head  attention  network  (Transformer).   The
functionality of a multi-head attention layer is to concatenate the attention weights of N
single-head attention layers and then apply a nonlinear transformation with a Dense layer.
Having the output of N single-head layers allows the encoding of multiple independent single-
head layer transformations into the model.  Therefore, the model can focus on multiple time
series steps at once.  Increasing the number of attention heads affects the ability of a model
to capture long-term dependencies [77].
For the transformer model to function properly, we need to attach the meaning of
time to our input features.  After we concatenate the input features to our transformer en-
coder, we encode the sequence of time (time embedding), which is hidden in our signal data.
We implement the existing Time2Vec method (equation 17) [132] for time embedding.  This
time embedding is a vector representation just like a normal embedding layer that can be
added to the neural network architecture to improve a model’s performance and overcome
the transformer’s temporal indifferences.  As shown in equation 17, the mathematical rep-
resentation of Time2Vec for the ideas of periodic and non-periodic patterns, as well as the
invariance to time rescaling, are used [132].

## 69
## 5.5 Self-supervised Learning
There  are  many  issues  regarding  the  technology  used  for  fall  detection  systems  in
the geriatric center of senior patients.  The fall detection system should preserve the privacy
of the patients and maximize the system’s flexibility.  The use of a wearable sensor device
with inertial sensors of accelerometer and gyroscope as the primary data source helps us
avoid those issues since it preserves anonymity.  The analysis of the signals obtained from
human body-mounted wearable sensors is commonly used to monitor the health status of
older  patients  with  movement  assistive  devices  [5].   These  sensors  usually  generate  com-
plex  hip  motion  signals,  which  are  difficult  to  interpret  without  expert  intervention.   A
computationally efficient fall detection modeling technique that will provide a meaningful
characterization  of  the  sensor  data  is  required  to  automatically  analyze  the  sensor  read-
ings to infer the kind of human activity performed by a user.  Many researchers have been
developing  supervised-based  fall  detection  methods  in  the  last  decades  [48],  [136],  [137].
However, there are still some limitations to the manual annotation of the dataset collected
using wearable sensors.
LeCun  proposed  self-supervised  learning  [19].   Self-supervised  learning  obtains  su-
pervisory  signals  from  its  dataset,  often  leveraging  the  underlying  structure  in  the  data.
The  general  technique  of  self-supervised  learning  is  to  predict  any  unobserved  or  hidden
part (or property) of the input from any observed or unhidden part of the input [19].  A
self-supervised approach can learn complex patterns using unlabeled data, achieving many
state-of-the-art results in different applications.  BERT is a pre-training learning model that
obtains state-of-the-art results in various NLP tasks [129], [138]).  Our proposed approach

## 70
is based on learning language representation self-supervised BERT [139].
Inspired by the success of BERT: pre-training (self-supervised) for language under-
standing [139] in natural language processing (NLP), this section explores pre-trained-based
fall detection systems on wearable sensor datasets.  Two different deep learning models are
used as baseline models- LSTM Encoder and a Transformer Encoder.  The SDSU dataset
[83] contains unlabeled and labeled fall datasets.  First, the labeled and unlabeled datasets
are required pre-processing and slide windowing steps.  Lastly, two base models of LSTM
and Transformer encoders are used for pre-training and fine-tuning wearable sensor signals.
## 5.5.1 Baseline Models
Self-supervised learning is a good technique for learning features in the absence of
sufficient labeled datasets.  It is very efficient where labeling data is expensive,  as in our
case, it needs to install a camera for manually annotating.  In this section, a self-supervised
fall detection model is proposed, as shown in Figure 20 that extracts representative features
learned from unlabeled data.  During pre-training, the base Encoder models leverage fea-
tures learned on unlabeled data.  After pre-trained, they will fine-tune the learned weights
according to the labeled fall dataset.  LSTM and Transformer encoders serve as base models
for pre-training and fine-tuning the SDSU dataset.
5.5.2 Pre-Training (Training on unlabeled data)
Transfer  learning  from  a  pre-trained  model  is  one  type  of  self-supervised  learning
method.   Self-supervised  learning  aims  to  extract  the  useful  underlying  representation  of
unlabeled  data  and  transfer  these  learned  data  representations  to  downstream  tasks.   In

## 71
Preprocessing and Windowing
## Unlabelled Data
## Labelled Data
Preprocessing and Windowing
Fine-Tuning or Training
## Pre-training
LSTM/Transformer
## Encoder
LSTM/Transformer
## Encoder
Fig. 20.  The self-supervised architecture.
this way, it can solve the problem of labeled data shortage.  The work exploits LSTM, and
Transformer encoder base models, which solve the self-supervised task by forcing the models
to learn features to solve the gait analysis of fall and non-fall activities. To get the pre-trained
weights of the unlabeled data, Random shuffling augmentation for generating synthetic data
and calculating the cosine similarity between the original and generated data as shown in
Figure 21.  When training is finished, the pre-trained weights will be automatically saved
and fine-tuned or trained in the same architecture using datasets with labels.
The pre-trained weights of unlabeled fall datasets are used to establish a self-supervised

## 72
## Fig. 21.  Contrastive Learning Architecture.
critical feature extraction method that helps the network achieve better results. This process
avoids recording the subjects using cameras for manual annotations and does not require
any prior knowledge.  The pre-training weights are fine-tuned with small labeled data and
trained with a linear classifier on top of the model’s trained layers.  The pre-training details
are shown in Algorithm 3, described in the next page.
5.5.3 Fine-Tuning and Training (Training on labeled data)
In this case, there are two options for using the pre-trained weights:  fine-tuning and
training.  In fine-tuning, we train the last layer of the model, freezes the other layers, and

## 73
Algorithm 3Function [P]=Pre-training
input :UnlabeledA(t) = [A
x
(t),A
y
(t),A
z
(t)] andG(t) = [G
x
(t),G
y
(t),G
z
## (t)]
output:Pre-trained model weights
forall data sequence activities of the datasetdo
Normalize all feature columns using equation 1;
Perform overlapping slide window with 0.2s window size and 0.1s stride ;
end
Transform the input data into 3D format of Torch (num of samples * num of features * num
steps)
Perform data augmentation using Random shuffling.
Applyf(.) using the Encoders.
## Geth
i
andh
j
Maximize similarity betweenh
i
andh
j
using Contrastive loss.
Save the pre-trained weights
train for half of the epochs used during pre-training (50/100 epochs).  However, we re-train
the  whole  network  as  we  did  the  same  strategy  during  pre-training,  except  we  used  the
labeled data.
Using the labeled dataset, we re-train and fine-tune the network architectures using
the method shown in Algorithm 4.  The contrastive loss presented by Le Cunn[140] is used to
re-train or fine-tune the labeled datasets.  During supervised training of the labeled dataset,
LSTM and Transformer encoders are optimized end-to-end using a contrastive loss in Eq.
## 18:
## L
loss
## (W,Y,
## ̄
## X
## 1
## ,
## ̄
## X
## 2
## ) = (1−Y)
## 1
## 2
## (D
## W
## )
## 2
## + (Y)
## 1
## 2
{max(0,m−D
## W
## )}
## 2
## ,(18)
where Y term here specifies, whether the two given data points (X
## 1
andX
## 2
) are similar (Y=
0) or dissimilar (Y= 1).  TheD
## W
term in parenthesis is the similarity (or dissimilarity).
## D
## W
is  the  loss  of  similar  data  points,  the  distance  between  them;  if  two  data  points  are

## 74
Algorithm 4Function [F]=Fine-tuning or training
Input  :Pre-trained  weights  and  LabeledA(t)   =   [A
x
(t),A
y
(t),A
z
(t)]  andG(t)   =
## [G
x
(t),G
y
(t),G
z
## (t)]
Output:Classes of fall and non-fall
forall data sequence activities of the datasetdo
end
Normalize all feature columns using equation 1;
Perform overlapping slide window with 0.2s window size and 0.1s samples stride ;
ifthere is a single sample out of the 0.2s window labeled as fallthen
Label the whole observation window as fall
else
Label non-fall
end
Perform balancing techniques on minor classes
Transform the input data into 3D format of Torch(num of samples * num of features *
num steps)
f(.) =weights(f(.)).
## Applyf(labeldataset)
Apply the cross-entropy loss function
Get the classes of fall and non-fall
labeled as similar, we have to minimize the distance between them.  Otherwise, the distance
has a margin parameterm >0 to impose a lower bound on the distance between a pair of
samples with different labels.
## 5.6 Summary
This  chapter  proposed  a  raw  signal-based  fall  detection  study  using  several  deep-
learning  network  architectures.   First,  we  propose  three  models  (FCN,  LSTM-FCN,  and
ResNet)  that  are  based  on  1D  CNN  architecture.   Second,  we  present  attention  neural
networks by giving high attention weights to those windows that are rounded for the peak
signal to detect fall and fall-like daily activities.  Third, we designed a deep learning model
that combines a convolution-based feature extractor and deep neural network blocks,  the

## 75
LSTM block,  and the transformer encoder block,  followed by a position-wise feedforward
layer.  We found that combining the input sequence with the convolution-learned features
of different kernels tends to benefit the fall detection model.  Last, we use self-supervised
learning that pre-trains unlabeled data and fine-tunes using small labeled data.

## 76
## CHAPTER 6
## RESULTS
We use a series of performance metrics to compare the different methods available to
realize the computational blocks of the network.  F1 score is defined as the harmonic mean of
precision and recall.  The goal of the F1 score is to combine the precision and recall metrics
into  a  single  metric.   At  the  same  time,  the  F1  score  has  been  designed  to  work  well  on
imbalanced data.  The sensitivity of a machine learning model is a measure of its ability to
detect positive instances.  This is also known as the true positive rate (TPR) or recall.  We
use sensitivity to evaluate model performance because it allows us to see how many positive
instances the model correctly identified.  Models with high sensitivity will have fewer false
negatives, meaning fewer positives are missed.  Specificity measures the proportion of true
negatives that the model correctly identifies.  In other words, there are likely to be another
proportion of actual negatives that were predicted as positives and could be referred to as
false positive.
Balanced  accuracy  is  a  raw  accuracy,  where  each  sample  is  weighted  according  to
its actual class’s inverse prevalence.  This metric helps us deal with imbalanced datasets by
avoiding inflated performance estimates in our datasets.  If the classifier performs equally
well  on  either  class,  balanced  accuracy  reduces  to  the  standard  accuracy.   In  contrast,  if
the  classical  accuracy  is  above  chance  only  because  the  classifier  takes  advantage  of  an
imbalanced test set, then the balanced accuracy, as appropriate, will drop to
## 1
nclasses
## .

## 77
F1Score=
## 2∗(precision∗recall)
precision+recall
## (19)
## Balanced Accuracy=
Specificity+Sensitivity
## 2
## (20)
## Precision=
## TP
## TP+FP
## ,(21)
## Sensitivity=
## TP
## TP+FN
## ,(22)
## Specificity=
## TN
## TN+FP
## ,(23)
where TP is the number of True Positives, TN is True Negative, FN is the number of False
Negatives, and FP is False Positive.
## 6.1 Image Based Transfer Learning
The URFD datasets are divided into training and testing with 80% and 20%, respec-
tively.  After we train and test on the higher layers of the proposed CNN architectures above.
A comparison of image-based fall detection results is presented in Table 2, with the highest
performance  highlighted  in  bold.   The  pairwise  comparison  of  the  image-encoding  meth-
ods and the deep-learning models above shows that Alexnet with CWT encoding performs
better, followed by ResNet50 with CWT image inputs.

## 78
TABLE 2.  Comparing the accuracy of a different combination of image encoding and CNN
architectures.
Images Transformation    AlexNet    ResNet50    InceptionV3
## CWT96.4393.791.2
## GAF    91.1789.0287.32
## MTF    90.086.7885.93
## RP    85.388.687.1
As we can see from the result above,  the continuous wavelet image encoding with
AlexNet has better results than the other combinations.  Hence, we compared and analyzed
CWT trained on the AlexNet architecture with other fall detection methods.  The training
progress  over  the  iteration  number  for  the  AlexNet  model  is  shown  in  Figure  22.   The
experimental evaluation on the URFD dataset using the continuous wavelet and AlexNet
results in the confusion matrix shown in Figure 23; performance metrics are shown in Table
## 3.
The Threshold based and Support Vector Machine methods are compared with our
Transfer  Learning  (TL)  method.   The  first  method  was  proposed  in  [43]  and  is  denoted
as (TB). It uses a threshold on the acceleration magnitude in order to detect the fall.  In
training, the threshold is determined as the minimum of the magnitude peaks during the fall
events.  In testing, the same threshold is used for detecting falls from non-falls.  Whereas the
second method, based on SVM, was proposed in [44] and originally used both accelerometer
data and Kinect depth data.  It initiates the fall detection procedure by using a threshold
value of 3g on the acceleration magnitude.  This fall detection procedure consists of an SVM
model trained on the extracted Kinect depth features that help to make the final decision
[78].  The depth features were scaled so that they had zero mean and unit variance.

## 79
Fig. 22.  Accuracy and loss over the training iteration.
The  TB  method  has  the  highest  amount  of  false  positives  (detecting  a  fall  when
no fall has occurred),  as it has the lowest specificity (90%).  It has the same accuracy as
the  SVM  method,  which  has  higher  specificity  but  lower  sensitivity.   This  result  means
that  the  SVM  method  produces  fewer  false  positives  but  at  the  dire  cost  of  more  false
negatives.   Meanwhile,  the  proposed  Transfer  Learning  (TL)  method  is  better  than  the
previous methods in all evaluation metrics except the sensitivity with the TB method (which
is 96.67% for TB and 95.0% for the TL method).  Even though the transfer learning relies
only on the accelerometer data, it has only produced a single false-positive result (specificity
96.875%), while the sensitivity of 95.83%. Moreover, the TB and SVM methods are invariant

## 80
TABLE 3.  Comparing the results of different methods.
Metrics    SVM    TBTL(Our Method)
## Accuracy    92.86    92.86    96.43
## Precision    94.17    90.00    95.83
## Sensitivity    90.096.67    95.83
## Specificity    95.090.096.875
Fig. 23.  Confusion Matrix for Image-based TL of the AlexNet Architecture.
to rotations as they didn’t use data augmentation used in our transfer learning method.
Better performance was achieved by combining the continuous wavelet analysis im-
age transformation with AlexNet architecture.  The wavelet-based time-frequency analysis
represented the sensor data by scalograms, whereby RGB images of those scalograms were
then suitable for input into the Alexnet deep learning framework.  Our method used the
accelerometer data only, but the results were better than the competing methods that used
both the Kinect and accelerometer data.  In addition, data augmentation was beneficial for
improving the classification.

## 81
## 6.2 One-dimensional Convolutional Neural Networks
This section presents the three 1D CNN-based deep-learning methods for fall detec-
tion based on acceleration and angular velocity from raw sensors.  The performance of those
different  classifiers  is  shown  in  Tables  4  and  5  on  shank  and  wrist  datasets,  respectively.
Three of the deep learning methods performed better on the shank-mounted datasets than
on wrist-mounted datasets,  which strengthens the findings of optimal location by Paolini
[71].  The AUC and F1-score of shank datasets are higher than wrist datasets in most deep
learning networks.  ResNet outperforms FCN and LSTM-FCN in most results, while FCN
is slightly better than LSTM-FCN.
The specificity is calculated to evaluate the system accuracy, defined as the probabil-
ity of near-fall activities occurring, given that the classifier predicts non-fall activities.  And
sensitivity is defined as the probability of fall activities occurring given that the classifier
predicts a fall.  It can be seen that the proposed architecture performed better in specificity
and sensitivity than the other architecture.
TABLE 4.  Comparing the different methods.
Metrics heightFCNLSTM-FCNResNet
## AUC94.5096.3097.43
## Sensitivity93.9197.1896.37
## Specificity95.0095.6598.17
F1-Score91.6394.7496.38
Different classifier methods are applied to datasets acquired and tested by Paolini et
al.  on the datasets are tested by Paolini [71].  They claim that the LSTM method yields a
better result, namely 91% on the wrist dataset and 92% on the shank dataset.  The authors

## 82
use 183 wrist and shank-based training features to compare machine learning methods, while
coping with imbalances in the datasets.  As we discussed in our proposed methods above, the
imbalanced nature of the datasets is addressed using variable-rate data sampling methods.
In Table 6, our results are compared with the result provided by Paolini [71].  Our results
from three of the deep learning methods method slightly outperform the LSTM classifier of
## Paolini [71].
TABLE 5.  Comparing the different methods on the Wrist sensor signal.
Metrics heightFCNLSTM-FCNResNet
## AUC93.5096.096.8
## Sensitivity93.0196.096.67
## Specificity94.0096.096.97
F1-Score91.6394.7495.87
TABLE 6.  Comparing the different model performances with existing methods.
## Our Methods
MetricsLSTM [71]FCNLSTM-FCNResNet
AUC (Shank)92.094.5096.3097.43
AUC (Wrist)91.093.5096.0096.8
The proposed ResNet network identified the falls against the non-fall activity with
an average F-1 score of 96.38% for the shank datasets and 95.87% for the wrist datasets.
These performances show that our proposed approach has slightly better results while using
only six features (three acceleration and three angular velocity features).
## 6.3 Attention Neural Network
The  public  Sisfall  [81]  dataset,  consists  of  19  ADLs  and  15  falls  performed  by  23

## 83
adults (11 male with and 12 female) and 15 (8 male and 7 female) elders.  The data is col-
lected using waist mounted embedded device with two accelerometers and one gyroscope.
Our proposed method uses one set of accelerometer and gyroscope data, which consists of
3D  acceleration  data  and  3D  angular  velocity.   We  calculated  specificity  to  evaluate  our
system accuracy, defined as the probability of near-fall activities occurring, given that the
classifier predicts non-fall activities.  Sensitivity is defined as the probability of fall activities
occurring given that the classifier predicts a fall.  It can be seen that our models performed
better in specificity and sensitivity than the other models.
TABLE 7.  Comparing the results of different methods.
## Specificity    Sensitivity    Accuracy
Our Methods(Attention NN)    0.970.980.98
## CNN-LSTM [76]    0.930.940.94
Table  7  above  shows  the  performance  of  the  proposed  model  against  a  competing
CNN-LSTM method by Nait [76].  The competing method [76] is implemented to compare
the  results  on  the  same  dataset,  as  the  dataset  used  by  the  author  is  not  public.   The
computing  model  is  compared  with  the  proposed  network  considering  the  computational
time for a real-time fall detection system as well as performance measures in Table 4.  It can
be observed that, in almost all cases, the proposed network obtained better results by taking
advantage of the maximum signal information.  The highest performance is achieved with
0.98 specificities, 0.97 sensitivity, and 0.98% accuracy.  From the results, it can be concluded
that attention-based networks, which are mostly used for NLP tasks, can be used to detect
falls with high specificity and sensitivity.  In particular, the specificity and sensitivity results

## 84
show that the peak-windowed attention network provides a better distinction between falls
and fall-like activities.
These results demonstrate the results of an attention neural network applied to accel-
eration data, where fall events have occurred in part of the dataset.  We take the maximum
value  of  the  sensor  signals  to  define  the  detector’s  observation  window.   Powered  by  a
transformer with word embedding, attention networks have achieved state of art in natu-
ral language processing (NLP) tasks.  Beyond the success of the transformer in efficiently
processing long sequences, this approach supports parallel computing with fast computation.
6.4 Compare LSTM and Transformer Encoders
In  this  section,  the  performance  of  our  proposed  method  of  LSTM-Encoder  and
Transformer-Encoder is compared with LSTM [71] and CNN-LSTM [76].  This experiment
proposed  a  fall  detection  study  using  competing  deep  learning  methods  that  use  multi-
windowing and competing methods of balancing class data samples.  The existing LSTM
and CNN-LSTM methods are implemented to compare them with our methods while eval-
uating them using SDSU dataset.  Table 8 compares the best results of the LSTM encoder,
and transformer encoder with those of [76] and [71].  Compared to [71], [76], which reported
mean F1-scores of 0.90 and 0.92, the proposed LSTM-encoder performs better with a mean
F1-score of 0.95 for shank datasets.  Moreover, the proposed transformer encoder method
with multi-window segmentation identified falls against a non-fall activity with an average
F-1  score  of  0.94  for  the  shank  datasets.   These  performances  suggest  that  our  proposed
approach improves the results by using multi-windowing segmentation and customized loss

## 85
function for handling the imbalanced datasets despite using only six features (three accel-
eration and three angular velocity features).
TABLE 8.  Comparison of our proposed methods against existing methods.
Balancing TechniquesF1-score    Balanced Accuracy
## LSTM [71]0.900.92
## CNN-LSTM [76]0.920.94
Our proposed (Transformer-based)0.940.95
Our proposed (LSTM-based)0.950.96
6.5 Comparing Self-supervised LSTM and Transformer Encoders
The self-supervised learning for fall detection was evaluated based on acceleration
and  angular  velocity  sensors.   The  experiment  has  been  implemented  using  the  Pytorch
framework.  The labeled datasets used for training and fine-tuning are divided into training,
validation,  and testing with 50%,  20%,  and 30%,  respectively [141].  We ran the training
twenty times to get the average results with a learning rate of 0.001, a batch size of 64, and
100 epochs.  The performance of those different classifiers is shown in Tables 9 and 10.  The
balanced accuracy and F1 score of the LSTM-based self-supervised model are higher than
those of the Transformer self-supervised model.  Furthermore, the self-supervised methods
performed better than supervised learning for both baseline models- LSTM and Transformer
encoder.
Regarding the self-supervised methods, training the pre-trained baseline models from
scratch provides better performance in most of the results than fine-tuning the pre-trained
baseline model.  Even though random oversampling does not add new datasets, as it simply

## 86
TABLE  9.   Comparing  the  LSTM-Encoder  and  Transformer-Encoder  base  models  of  fall
detection.
MetricsSupervisedSelf-supervised
trainingFine-TuningTraining
LSTMTransformerLSTMTransformerLSTMTransformer
## Balanced
## Accuracy
## 0.910.930.950.940.970.96
F1-Score0.950.960.960.960.960.94
TABLE 10.  Comparing the supervised and Self-supervised methods of LSTM-Encoder for
fall detection.
MetricsSupervised LSTM-EncoderSelf-supervised LSTM-Encoder
## Balanced Accuracy0.970.98
## F1-score0.960.97
duplicates  random  examples,  the  results  are  slightly  better.   In  most  results,  the  ResNet
base model outperforms the FCN base model, as shown in Table 9.  Furthermore, random
over-sampling  with  the  pre-trained  ResNet  baseline  model’s  training  outperformed  other
methods combinations in both of the performance metrics, as we can see in Table 10 and
the confusion matrix in Figure 24.

## 87
Fig. 24.  Confusion matrices for the different methods, LSTM [71], CNN-LSTM [76], Trans-
former encoder (our method) and LSTM encoder (our method), respectively.

## 88
Fig. 25.  Sample predicted classes using the LSTM encoder-based model.  Non-fall activity
(class 0), fall activity (class 1), and True, Pred are the actual and predicted classes.

## 89
Lastly, we visualized the actual and predicted classes of the LSTM-Encoder network
for the sample data points.  As shown in Figure 25, the visualization shows the data points
and their corresponding predicted and actual classes. In the third column of the first row, the
ADL, a near-fall event is correctly classified as non-fall.  It shows that our LSTM-Encoder
network is accurate and robust even for near-fall events.
The  experiments  have  been  implemented  using  the  PyTorch  framework  [142]  and
the  labeled  datasets  used  for  training,  validation,  and  testing  with  50%,  20%,  and  30%,
respectively.
Furthermore, we conducted five different experiments so that each experiment was
designed with specific goals, including comparing with existing fall detection methods, find-
ing the sensor types for fall detection,  and comparing different techniques to balance the
fall datasets.  The experiment and results obtained are discussed below.
## 6.6 Sliding Window Segmentation Methods
This experiment aims to evaluate the impact of windowed methods segmentation on
deep learning-based fall detection approaches.  How segmentation impacts fall datasets has
not yet been evaluated.  An inappropriate data segmentation method, such as a conventional
sliding window [42], can decrease the classifier’s overall performance.  We found that using
multi-window segmentation increased the F1 score for our dataset, as supported in Table
-  There is a difference in the length of fall data between the different datasets, and the
highest F-score can be obtained when the window size is the same as the fall data.  When
the length of the fall data is known and uniform, selecting the window size is trivial.  Real-
world  constraints,  however,  imply  fall  data  temporal  restrictions  shorter  than  the  above

## 90
studies.  As this dataset has a varied length of fall data, the results from our dataset, shown
in  Table  11,  can  more  accurately  reflect  the  classifier’s  performance  in  real  time.   Multi-
window segmentation yields a better result than the other two segmentation methods:  peak
detection sliding window and conventional sliding window.
Although conventional sliding windows use overlapping windows, the window signal
for a fall event does not always align perfectly with the fall patterns observed in the input
signals.  As a result, our classifiers would ordinarily produce false negatives and false pos-
itives.  To solve this problem, we propose more robust data segmentation methods.  When
windowing from peak signals, the window is aligned with the fall patterns of the input sig-
nals.   Nevertheless,  capturing  the  entire  fall  in  one  observation  window  remains  difficult.
Using multi-windowing, we can segment the data according to the class of labels.  Multi-
windowing  segmentation  not  only  provides  better  results  but  also  helps  detect  falls  on  a
real-time  basis  (0.2  seconds).   Because  the  data  is  segmented  based  on  the  label,  we  can
make the input (observation windows) appropriately small.
TABLE 11.  LSTM-Encoder results for the different segmentation methods.
Segmentation MethodsF1-score    Balanced Accuracy
## Conventional Sliding Window [42]0.900.92
Peak-Detection Window0.930.94
## Multi-window Sliding0.960.97
6.7 Compensating an Imbalanced Dataset
This experiment aimed to compare the robustness of the LSTM-Encoder architecture
for imbalanced datasets based on competing imbalance-handling techniques.  We compare

## 91
different methods for handling imbalanced datasets.  The customized loss function and data
augmentation improve the specificity and sensitivity of the architecture, resulting in a bal-
anced accuracy of 0.92 and 0.98, respectively.  However, both resampling techniques didn’t
help us reduce the number of false positives and false negatives.  Multi-window segmenta-
tion data from the shin-mounted sensor are used to train those balancing techniques.  Table
12 shows how all the methods were performed in the evaluation set.  In contrast to image
classification problems, our results indicate that under-sampling and over-sampling did not
help sensor signals.
TABLE 12. LSTM-Encoder results for the various balancing methods of the dataset samples.
Balancing TechniquesF1-score    Balanced Accuracy
## Under-sampling0.840.87
## Over-sampling0.870.89
## Data Augmentation0.930.94
## Customized Loss Function0.960.97
## 6.8 Sensor Positions
The  purpose  of  this  experiment  is  to  demonstrate  the  capability  of  the  proposed
LSTM-Encoder network for predicting the optimal position of the sensors for fall detection
based on the observation window generated with the help of multi-window sliding.  There-
fore, we train an LSTM-Encoder network using fall datasets (accelerometers and gyroscopes)
from all 16 sensor positions and evaluate it using the test set.  The average of the left and
right  sensor  positions  of  the  forearm,  upper  arm,  hand,  shank,  hip,  and  foot  is  used  to
calculate the performance of each anatomical location.  Furthermore, the thoracic result is

## 92
the average of the upper and lower thoracic locations.  We measure the model’s balanced
accuracy  and  F1  score  to  evaluate  its  performance.   Based  on  the  LSTM  encoder-based,
we present the results for various sensor positions in Table 13.  For the shank position, the
network achieves a remarkable F1-score of 0.97.  In general, the F1-score of the network is
higher than 0.90, except for the foot and hand, where the F1-score is 0.88 and 0.85.  The
results  indicate  that  different  sensor  locations  give  us  various  performances  and  that  the
shank is the optimal location for the sensors.
TABLE 13.  LSTM-Encoder results for the various sensor positions.
## Sensor Positions    F1-score    Balanced Accuracy
## Foot0.880.90
## Shank0.960.97
## Pelvic0.940.92
## Hand0.850.84
## Forearm0.930.94
## Upperarm0.950.95
## Head0.900.91
## Hip0.930.92
## Thoracic0.940.95
## 6.9 Sensor Types
The  data  obtained  from  the  three-axis  accelerometer  and  gyroscope  are  combined
to obtain information about global acceleration and orientation of the body, which in turn
is  an  input  into  the  algorithm.   One  of  the  objectives  of  this  study  was  to  examine  and
compare the relative effectiveness of accelerometer-based algorithms and accelerometer-plus-
gyroscope-based algorithms intended for fall detection as part of the comparative study.  In
the  near-fall  dataset,  we  find  that  the  algorithm  that  integrates  gyroscope  signals  has  a

## 93
markedly better performance than the algorithm bereft of these inputs.  As shown in Table
14, Using a gyroscope in conjunction with an accelerometer as part of the detection process
can potentially lead to incremental benefits compared to using accelerometry alone for the
detection process.
Many fall detection studies [143] utilize an accelerometer as a primary sensor to de-
termine falls.  However, using only the acceleration measurements can result in many false
positives  and  false  negatives  caused  by  near-fall  activities  such  as  sitting  down  fast  on  a
mattress.  Near-fall and fall events have almost the same vertical signal variation, making
it  difficult  to  differentiate.   However,  the  gyroscope’s  angular  velocity  measurements  can
significantly reduce false positives and negatives caused by near-fall activities.  Hence,  to
detect falls with low energy consumption, we advocate the consideration of the acceleration
along the x, y, and z-axis and angular velocity along the x, y, and z-axis.
TABLE 14.  LSTM-Encoder results for the different sensor types.
Balancing TechniquesF1-score    Balanced Accuracy
## Accelerometer0.900.92
## Accelerometer + Gyroscope0.960.97
Therefore, we use acceleration along the x, y, and z axes to detect falls and angular
velocity along the x, y, and z axes.  The evidence gathered in this study suggests that assess-
ing wearable sensors located on the shins through acceleration and angular velocity features
may  represent  an  optimal  combination  to  discriminate  fallers  from  non-fallers  (including
near-falls).

## 94
## 6.10 Window Size Impact
In most of the previous studies [64], [79], an input sequence based on sliding windows
uses a 1-3 second window size.  The data sequence is first segmented into sliding windows of
a certain duration for feature extraction and annotated for training a deep learning model.
The selection of the appropriate size of a sliding window is crucial for finding temporal fea-
tures from the sensor data.  The main factors for selecting the window size are restrictions
in sampling the accelerometry data, recognition speed, and accuracy.  Due to the lack of a
well-annotated standard dataset for comparison of the detection models, various annotation
techniques have been used for feature extraction from the sensor dataset.  The duration of
the fall event is not always the same [42], [79], [92]; as a result, the long duration of the fall
sequence is an important factor that affects the annotation of the windows- representing the
fall segment entirely.  Using a large input sequence of 1-3 second window size gives us good
accuracy, but insufficient computational efficiency to be used as real-time fall detection [42],
## [79], [92].
TABLE 15.  LSTM-Encoder results for different window sizes.
Win Size (in sec)    Conventional Windowing    Multi-windowing
## 30.970.98
## 20.950.98
## 10.930.98
## 0.50.920.97
## 0.20.900.96
## 0.10.870.92
As we can see from Table 15 and Figure 26, the small window size with the conven-
tional sliding window-based method has less accuracy because they have restricted sample

## 95
input sequences and do not always align with the actual fall segments- to be annotated as
fall.  Some public datasets’ sampling rate is 100 Hz signal- 100 samples per second.  In the
real-time fall detection of 0.2-second signals, the input sequence is around 20 samples.  When
the features are selected from the small-size sliding window, they represent the fall segment
precisely only if the fall segment lies within the sliding window.  Annotating becomes diffi-
cult when multiple windows share a falling segment.  However, it is necessary to use a small
input  sequence  for  real-time  fall  detection.   To  overcome  these  limitations,  we  generated
dense data points with small windows size using multi-window sliding segmentation.  In our
work, the window size is 0.2 seconds, with the multi-window moving along the time axis for
real-time fall detection.
Fig. 26.  LSTM-Encoder results for different window sizes.

## 96
## 6.11 Performance Comparison
In this section, we research the performance comparison of the different deep learn-
ing models on eight datasets.  We develop a method that helps us to compare the different
performances of the existing fall detection methods with our proposed methods to decide
which ones perform best.  Some fall detection findings suggest that the performance of the
proposed networks,  Transformer-Encoder and LSTM-Encoder,  improved significantly.  To
demonstrate improve performance over existing work, one should not only consider the dif-
ference  in  accuracy  between  the  two  methods,  but  also  conduct  a  statistical  significance
test.  Demsar [144] defined a Critical Difference (CD) diagram that provides an interesting
visualization of the statistical significance of the observed paired differences between a set
of datasets on a set of detection tasks.  They allow us to compare all methods against each
other on this set of tasks and validate the results of all these paired comparisons.  Through
comparison differences, we resent comparative performance results for several machine learn-
ing classifiers across the different datasets, as shown in Figure 27.  This comparison results
demonstrate a rigorous fall detection process intended to guide future analyses of fall detec-
tion systems.
A  dataset  of  100  Hz  fall  signals  was  used  to  compare  and  train  the  deep-learning
networks.   The  following  preprocessing  is  proposed  to  use  the  networks  for  signals  with
different sampling frequencies.  Assume the input signal frequencyAis (A
## 1
## ,A
## 2
## ,..,A
n
), and
the network is trained on signals with the frequencyλ.  Thus,T=
n
v
is the signal duration.
The input signal should be converted as follows [145].
•Form a vector of time samplest= (t
## 1
## ,t
## 2
## ,...,t
n
), whereti=
(2i−1)T
## 2n
are the midpoints

## 97
of  the  time  intervals  formed  by  dividing  the  segment  [0,T]  into  n  equal  parts  (i=
## 1,2,....,n).
•On the set of points (t
## 1
## ,A
## 1
## ),(t
## 2
## ,A
## 2
## ),...,(t
n
## ,A
n
), construct the cubic spline [146].
•Form the vector of new time samples to get thist
## ′
## = (t
## ′
## 1
## ,t
## ′
## 2
## ,...,t
## ′
m
), wherem=⌈λT⌉,
t
## ′
i
## =
(2i−1)T
## 2m
## .
•Using the cubic spline, find the signal values att
## ′
.  The resulting signal sequence will
be input into the deep learning models.
Fig. 27.  Performance comparison of eight datasets using six deep learning methods.
The critical difference (CD) diagrams show the mean ranks of each model for  the
eight fall datasets.  The lower the rank (further to the right), the better the performance of
a model compared to the others on average.  A line in each diagram indicates no significant
difference in performance among the models crossed by that line.  This is in terms of the
Friedman test that compares the ranks of multiple classifiers [144].
As we can see from the CD diagram above, the LSTM encoder with Multi-window
segmentation performs better than the other methods overall on the eight different datasets.
LSTM and Transformer encoders with Multi-windowing are the two best encoders in the
Figure.  There is a statistically significant difference.  The Transformer encoder and ResNet

## 98
perform better than the LSTM encoder on a few datasets.  The existing methods, LSTM
and CNN-LSTM with a conventional sliding window, are the worst-performing networks.

## 99
## CHAPTER 7
## CONCLUSION AND FUTURE WORKS
This  chapter  concludes  the  works  presented  in  this  dissertation  and  outlines  the
future works of fall detection research.
## 7.1 Conclusion
Gait  disorders,  a  highly  prevalent  condition  in  older  adults  of  age  older  than  65
years, is a deviation from normal gait performance that leads to gait instability and risk of
falling.  Falls are a major cause of injuries and hospital admissions among elderly people.
Thus,  the  caregiving  process  and  the  quality  of  life  of  older  adults  can  be  improved  by
enabling the detection of falling events for the elderly, monitoring the patient, and conse-
quently providing emergency support in the event of falling.  This dissertation focuses on
detecting the impending fall risk of senior subjects within the geriatric population towards
a planned approach to mitigating fall injuries through pneumatic airbag deployment.  With
the widespread adoption of wearable sensors, there is an increased emphasis on fall predic-
tion models that effectively cope with accelerometry signal data.  Fall detection and gait
classification are challenging tasks, especially in differentiating falls from near-falls.
This dissertation proposes different deep-learning methods for fall detection and gait
classification.   First,  in  chapter  4,  a  fall  detection  algorithm  using  transfer  learning  and
image  encoding  of  a  triaxial  accelerometer  by  using  the  pre-trained  CNN  architecture  is
demonstrated.  These methods significantly improve the results from the classical machine

## 100
learning methods for public fall datasets.  Second, in Chapter 5, various deep-learning meth-
ods are developed for training raw sensor signal datasets.  This method demonstrated that
combining dataset balancing methods and several sliding window segmentation with deep
learning models significantly boosts the performance of our models.  The first method in-
troduced in this chapter is applying attention to the neural network analysis of acceleration
data where a fall is known to have occurred.  Peak-window segmentation of sensor signals
is  used  to  define  the  observation  window  sliding.   Specifically,  this  method  improves  the
performance  in  accuracy  and  computational  cost.   Next,  convolutional  feature  extractors
with LSTM and Transformer encoders are used.  The extracted features are integrated with
the  deep  learning  method  used  for  the  generalizing  analysis  of  the  kinematic  parameters
(such as acceleration and gyroscope).  Finally, the dissertation concludes with Chapter 6,
presenting different results.  Raw signals are processed for training the deep learning models.
Our performance results are compared with existing methods on a variety of datasets using
the Critical Difference (CD) diagram [144].  The performance comparison using the Critical
Difference (CD) diagram and window impact method are important for developing a robust
fall detection method for the research community.
## 7.2 Future Work
Although  using  wearable  sensors  has  the  advantage  of  flexibility  and  privacy  over
camera-based fall detection,  camera video recording is still used for manual data annota-
tion.  Data labeling for supervised machine learning methods is expensive, as we use video
recording and playback of each patient’s video to label the data.  Future work of the fall de-
tection study should employ self-supervised and unsupervised machine learning algorithms.

## 101
It is feasible to create a large database by segmenting input sequences from different posi-
tions with different sensors using multi-window or peak detection windowing techniques and
feed it to a variational Auto-encoder with LSTM and Transformer to perform unsupervised
clustering.

## 102
## REFERENCES
[1]    WHO. “Falls.” (2019), [Online]. Available:http://www.who.int/en/news-room/
fact-sheets/detail/falls.
[2]    Statista. “USA seniors as a percentage of the population 1950-2050,” Statista Re-
search  Department.  (2021),  [Online].  Available:https : / / www . statista . com /
statistics / 457822 / share - of - old - age - population - in - the - total - us -
population/.
[3]    B.  Moreland,  R.  Kakara,  and  A.  Henry,  “Trends  in  nonfatal  falls  and  fall-related
injuries  among  adults  aged  65  years  —  united  states,”MWR  Morb  Mortal  Wkly
Rep, vol. 69, pp. 875–881, 2020.
[4]    B.  Johnson,  D.  Hayes,  K.  Brown,  H.  Elizabeth,  and  A.  Kathleen.  “CDC  national
health report: Leading causes of morbidity and mortality and associated behavioral
risk  and  protective  factors—united  states.”  (2013),  [Online].  Available:https://
www.cdc.gov/mmwr/preview/mmwrhtml/su6304a2.htm.
[5]    K.  Bright  and  L.  Coventry,  “Assistive  technology  for  older  adults:  Psychological
and  socio-emotional  design  requirements,”International  Conference  on  Pervasive
Technologies Related to Assistive Environments, pp. 1–4, 2013.
[6]    D. Lim, C. Park, N. Kim, S. Kim, and Y. Yu, “Fall-detection algorithm using 3-axis
acceleration: Combination with simple threshold and hidden markov model,”Journal
of Applied Mathematics, 2014.
[7]    X. Wang, J. Ellul, and G. Azzopardi, “Elderly fall detection systems: A literature
survey,”Frontiers in Robotics and AI, vol. 7, 2020.
[8]    D. John, Q. Tang, F. Albinali, and S. S. Intille, “An open-source monitor-independent
movement  summary  for  accelerometer  data  processing.,”Journal  for  the  measure-
ment of physical behavior, vol. 2 4, pp. 268–281, 2019.
[9]    D. Ravi, C. Wong, B. Lo, and G. Yang, “Deep learning for human activity recognition:
A resource efficient implementation on low-power devices,”in Proc. 2016 IEEE 13th
Int. Conf. Wearable Implantable Body Sensor Netw., pp. 71–76, 2016.
[10]    M.  Zeng,  L.  T.  Nguyen,  B.  Yu,et  al.,Convolutional  Neural  Networks  for  Human
Activity Recognition using Mobile Sensors. Moffett Field, CA, USA.: Department of
Electrical and Computer Engineering, Carnegie Mellon University, 1996.
[11]    A. Purwar, D. Jeong, and W. Chung, “Activity monitoring from real-time triaxial
accelerometer data using sensor network,”IEEE  Int.  Conf.  Control,  Autom.  Syst.,
pp. 2402–2407, 2007.

## 103
[12]    A.  Vaswani,  N.  Shazeer,  N.  Parmar,et  al.,  “Attention  is  all  you  need,”NIPS,
pp. 2921–2929, 2017.
[13]    Y. Tatsunami and M. Taki, “Sequencer: Deep lstm for image classification,”ArXiv,
vol. abs/2205.01972, 2022.
[14]    J.  Deng,  W.  Dong,  R.  Socher,  J.  Li,  K.  Li,  and  L.  Fei-Fei,  “Imagenet:  A  large-
scale hierarchical image database.,”IEEE Computer Vision and Pattern Recognition
## (CVPR), 2009.
[15]    B. Heo, S. Yun, D. Han, S. Chun, J. Choe, and S. J. Oh, “Rethinking spatial dimen-
sions  of  vision  transformers,”2021  IEEE/CVF  International  Conference  on  Com-
puter Vision (ICCV), pp. 11 916–11 925, 2021.
[16]    A.  Dosovitskiy,  L.  Beyer,  A.  Kolesnikov,et  al.,  “An  image  is  worth  16x16  words:
Transformers for image recognition at scale,”ArXiv, vol. abs/2010.11929, 2020.
[17]    Z. Cui, R. Ke, and Y. Wang, “Deep bidirectional and unidirectional lstm recurrent
neural network for network-wide traffic speed prediction,”ArXiv, vol. abs/1801.02143,
## 2018.
[18]    L. Alzubaidi, J. Zhang, A. J. Humaidi,et  al., “Review of deep learning: Concepts,
cnn architectures, challenges, applications, future directions,”Journal  of  Big  Data,
vol. 8, 2021.
[19]    Yann  LeCun,Self-supervised  learning:  The  dark  matter  of  intelligence,https://
ai.facebook.com/blog/self-supervised-learning-the-dark-matter-of-
intelligence/, Accessed May 23, 2022, Meta AI, 2021.
[20]    M. Sharma, M. Kumari, and V. B. Singh, “Understanding the meaning of bug at-
tributes and prediction models,” inI-CARE ’13, 2013.
[21]    G.  Piccinini,  “The  first  computational  theory  of  mind  and  brain:  A  close  look  at
mcculloch and pitts’s “logical calculus of ideas immanent in nervous activity”,”Syn-
these, vol. 141, Aug. 2004.doi:10.1023/B:SYNT.0000043018.52445.3e.
[22]    Y. LeCun, L. Bottou, G. B. Orr, and K.-R. M ̈uller, “Efficient backprop,” inNeural
Networks: Tricks of the Trade, This Book is an Outgrowth of a 1996 NIPS Workshop,
Berlin, Heidelberg: Springer-Verlag, 1998, pp. 9–50,isbn: 3540653112.
[23]    K. Fukushima, “Neocognitron: A self-organizing neural network model for a mecha-
nism of pattern recognition unaffected by shift in position,”Biological  Cybernetics,
vol. 36, pp. 193–202, 1980.
[24]    L. A. Gatys, A. S. Ecker, and M. Bethge, “Image style transfer using convolutional
neural networks,”2016 IEEE Conference on Computer Vision and Pattern Recogni-
tion (CVPR), pp. 2414–2423, 2016.

## 104
[25]    B. Liu, H. Cai, Z. Zhang,et  al., “More is less: Domain-specific speech recognition
microprocessor using one-dimensional convolutional recurrent neural network,”IEEE
Transactions  on  Circuits  and  Systems  I:  Regular  Papers, vol. 69,  no.  4, pp. 1571–
1582, 2022.doi:10.1109/TCSI.2021.3134271.
[26]    J. Du, L. Gui, Y. He, and R. Xu, “A convolutional attentional neural network for sen-
timent classification,” in2017 International Conference on Security, Pattern Analy-
sis, and Cybernetics (SPAC), 2017, pp. 445–450.doi:10.1109/SPAC.2017.8304320.
[27]    R.  C.  Staudemeyer  and  E.  R.  Morris,  “Understanding  lstm  -  a  tutorial  into  long
short-term memory recurrent neural networks,”ArXiv, vol. abs/1909.09586, 2019.
[28]    F.  A.  Gers,  urgen  Schmidhuber,  and  F.  Cummins,  “Learning  to  forget:  Continual
prediction with lstm learning to forget: Continual prediction with lstm,” 1999.
[29]    K.  Irie,  A.  Zeyer,  R.  Schl ̈uter,  and  H.  Ney,  “Language  modeling  with  deep  trans-
formers,” inInterspeech, 2019.
[30]    P.  Gao,  S.  Geng,  X.  Wang,  J.  Dai,  and  H.  Li,  “Scalable  transformers  for  neural
machine translation,”ArXiv, vol. abs/2106.02242, 2021.
[31]    A.  Hern ́andez  and  J.  M.  Amig ́o,  “Attention  mechanisms  and  their  applications  to
complex systems,”Entropy, vol. 23, 2021.
[32]    W.  Wei,  Z.  Wang,  X.  Mao,  G.  Zhou,  P.  Zhou,  and  S.  Jiang,  “Position-aware  self-
attention based neural sequence labeling,”Pattern Recognition, vol. 110, p. 107 636,
## 2021,issn: 0031-3203.doi:https://doi.org/10.1016/j.patcog.2020.107636.
[Online].  Available:https://www.sciencedirect.com/science/article/pii/
## S0031320320304398.
[33]    H. Ngai, Y. Park, J. Chen, and M. Parsapoor, “Transformer-based models for ques-
tion answering on covid19,”ArXiv, vol. abs/2101.11432, 2021.
[34]    R. M. Farsani and E. Pazouki, “A transformer self-attention model for time series
forecasting,” 2021.
[35]    Y. Wang, J. Xu, and Y. Sun, “End-to-end transformer based model for image cap-
tioning,” inAAAI Conference on Artificial Intelligence, 2022.
[36]    X. Chen, S. Zhang, D. Song, P. Ouyang, and S. Yin, “Transformer with bidirectional
decoder for speech recognition,” inInterspeech, 2020.
[37]    E. Casilari, R. Luque, and M. Moron, “Analysis of android device-based solutions
for fall detection,”Sensors (Basel), vol. 15(8), pp. 17 827–17 894, 2015.

## 105
[38]    W.  Teufl,  M.  Lorenz,  M.  Miezal,  B.  Taetz,  M.  Frohlich,  and  G.  Bleser,  “Towards
inertial sensor based mobile gait analysis: Event-detection and spatio-temporal pa-
rameters,”Sensors, vol. 19, p. 38, 2019.
[39]    P. Picerno, “25 years of lower limb joint kinematics by using inertial and magnetic
sensors: A review of methodological approaches,”Gait Posture, vol. 51, pp. 239–246,
## 2017.
[40]    V. Camomilla, E. Bergamini, S. Fantozzi, and G. Vannozzi, “Trends supporting the
in-field use of wearable inertial sensors for sport performance evaluation: A systematic
review,”Sensors, vol. 18, p. 873, 2018.
[41]    Q. Li, A. Stankovic, A. Hanson, T. Barth, J. Lach, and G. Zhou, “Fast fall detection
using gyroscopes and accelerometer-derived posture information,”Sixth International
Workshop on Wearable and Implantable Body Sensor Networks,J Ambient Intell Hu-
man Comput, pp. 138–143, 2009.
[42]    P. Edy, S. Putray, and R. Vesilo, “Window-size impact on detection rate of wearable-
sensor-based fall detection using supervised machine learning,”IEEE, 2017.
[43]    K. Bourke and M. Lyons, “A threshold-based fall-detection algorithm using a bi-axial
gyroscope sensor,”Med. Eng. Phys., vol. 30, pp. 84–90, 2008.
[44]    B. Kwolek and M. Kepski, “Human fall detection on embedded platform using depth
maps and wireless accelerometer,”Computer Methods and Programs in Biomedicine,
vol. 117, pp. 489–501, 2014.
[45]    S. Abbate, M. Avvenuti, P. Corsini, J. Light, and A. Vecchio, “Monitoring of human
movements for fall detection and activities recognition in elderly care using wireless
sensor network: A survey,” 2010.
[46]    L. Jorge, O. Reyes, A. S. Luca Oneto, X. Parra, and D. Anguita, “Transition-aware
human activity recognition using smartphones,”Neurocomputing. Springer, 2015.
[47]    P.  Tsinganos  and  A.  Skodras,  “A  smartphone-based  fall  detection  system  for  the
elderly,”Proceedings of the 10th International Symposium on Image and Signal Pro-
cessing and Analysis, 2017.
[48]    A. Ahmed, A. Jalal, and K. Kim, “Rgb-d images for object segmentation, localiza-
tion  and  recognition  in  indoor  scenes  using  feature  descriptor  and  hough  voting,”
2020  17th  International  Bhurban  Conference  on  Applied  Sciences  and  Technology
(IBCAST), pp. 290–295, 2020.
[49]    J. Guo, Y. Li, M. Hou, S. Han, and J. Ren, “Recognition of daily activities of two
residents in a smart home based on time clustering,”Sensors, vol. 20, no. 5, 2020,

## 106
issn: 1424-8220.doi:10.3390/s20051457. [Online]. Available:https://www.mdpi.
com/1424-8220/20/5/1457.
[50]    C. M. Ranieri, S. MacLeod, M. Dragone, P. A. Vargas, and R. A. F. Romero, “Ac-
tivity recognition for ambient assisted living with videos, inertial units and ambient
sensors,”Sensors, vol. 21, no. 3, 2021,issn: 1424-8220.doi:10.3390/s21030768.
[Online]. Available:https://www.mdpi.com/1424-8220/21/3/768.
[51]    N. Figueiredo, C. Leal, L. Pinto, J. Bolito, and A. Lemos, “Exploring smartphone
sensors for fall detection,”The Journal of Mobile User Experience, vol. 5, 2016.
[52]    M. Kangas, I. Vikman, J. Wiklander, P. Lindgren, L. Nyberg, and J ̈ams ̈a, “Sensitivity
and specificity of fall detection in people aged 40 years and over,”Gait  e  Posture,
vol. 29, pp. 571–574, 2009.
[53]    G. Shi, S. Chan, Y. Luo,et al., “Development of a human airbag system for falling
protection using mems motion sensing technology,”IEEE  Int.  Conf.  Intell.  Robots
Syst., pp. 4405–4410, 2006.
[54]    G. Shi, S. Chan, J. Li, S. Leung, Y. Zou, and Y. Jin, “Mobile human airbag system
for fall protection using mems sensors and embedded svm classifier,”IEEE Sensors,
vol. 9, pp. 495–503, 2009.
[55]    L. Montesinos, R. Castaldo, and L. Pecchia, “Wearable inertial sensors for fall risk
assessment and prediction in older adults: A systematic review and meta-analysis.,”
IEEE Trans Neural Syst Rehabil Eng., vol. 26(3), pp. 573–582, 2018.
[56]    J. Klenk, C. Becker, F. Lieken,et al., “Comparison of acceleration signals of simulated
and real-world backward falls,”Med. Eng. Phys., pp. 368–373, 2011.
[57]    Y. Hammerla, S. Halloran, and T. plotz, “Convolutional, and recurrent models for
human activity recognition using wearables,”Proceedings  of  the  25th  International
Joint Conference on Artificial Intelligence, IJCAI, pp. 1533–1540, 2016.
[58]    G. Leoni, P. Endo, K. Monteiro, E. Rocha, I. Silva, and T. Lynn, “Accelerometer-
based  human  fall  detection  using  convolutional  neural  network,”Med.  Eng.  Phys,
vol. 19, p. 1644, 2019.
[59]    J. Dai, X. Bai, Z. Yang, Z. Shen, and D. Xuan, “Perfalld: A pervasive fall detection
system using mobile phones,”Proceedings of the 8th IEEE International Conference
on Pervasive Computing and Communications (PERCOM 2010), 2010.
[60]    W. Lee and J. Carlisle, “Detection of falls using accelerometers and mobile phone
technology,”Age and Ageing, vol. 40, 2011.
[61]    V. Viet and D. Choi, “Fall detection with smart phone sensor,”The 3rd International
Conference on Internet (ICONI) 2010, 2011.

## 107
[62]    A. Ozdemir and B. Barshan, “Detecting falls with wearable sensors using machine
learning techniques,”Sensors, vol. 14, pp. 10 691–10 708, 2014.
[63]    A. Ramachandran and A. Karuppiah, “A survey on recent advances in wearable fall
detection systems,”BioMed  Research  International, vol. 2020, pp. 1–17, 2020.doi:
## 10.1155/2020/2167160.
[64]    I. Putra, J. Brusey, E. Gaura, and R. Vesilo, “An event-triggered machine learning
approach for accelerometer-based fall detection,”Sensors, vol. 18, no. 2, p. 20, 2017.
doi:10.3390/s18010020.
[65]    K.-C.  Liu,  C.-Y.  Hsieh,  S.  J.-P.  Hsu,  and  C.-T.  Chan,  “Impact  of  sampling  rate
on wearable-based fall detection systems based on machine learning models,”IEEE
Sensors  Journal, vol. 18, no. 23, pp. 9882–9890, 2018.doi:10.1109/jsen.2018.
## 2872835.
[66]    C. Medrano, R. Igual, I. Plaza, and M. Castro, “Detecting falls as novelties in ac-
celeration patterns acquired with smartphones,”PLoS ONE, vol. 9, no. 4, C. Lovis,
Ed., e94811, 2014.doi:10.1371/journal.pone.0094811.
[67]    S.  Abbate,  M.  Avvenuti,  F.  Bonatesta,  G.  Cola,  P.  Corsini,  and  A.  Vecchio,  “A
smartphone-based fall detection system,”Pervasive and Mobile Computing, 2012.
[68]    L. Tong, Q. Song, Y. Ge, and M. Liu, “Hmm-based human fall detection and pre-
diction method using tri-axial accelerometer,”IEEE SENSORS, vol. 13, p. 5, 2013.
[69]    C. Szegedy, V. Vanhoucke, S. Ioffe, J. Shlens, and Z. Wojna, “Rethinking the incep-
tion architecture for computer vision,”CoRR, vol. abs/1512.00567, 2015.
[70]    A. Krizhevsky, I. Sutskever, and G. E. Hinton, “Imagenet classification with deep
convolutional neural networks,”26th Annual Conference on Neural Information Pro-
cessing Systems, Lake Tahoe, Nevada, United States, pp. 1106–1114, 2012.
[71]    C. Paolini, D. Soselia, H. Baweja, and M. Sarkar, “Optimal location for fall detection
edge inferencing,”IEEE Global Communications Conference (GLOBECOM), vol. 8,
pp. 1–6, 2019.
[72]    F.  Bagal`a,  C.  Becker,  A.  Cappello,et  al.,  “Evaluation  of  accelerometer-based  fall
detection algorithms on real-world falls,”PLoS  ONE, vol. 7, no. 5, A. Bayer, Ed.,
e37062, 2012.doi:10.1371/journal.pone.0037062.
[73]    S. Chaudhuri, D. Oudejans, H. J. Thompson, and G. Demiris, “Real-world accuracy
and use of a wearable fall detection device by older adults,”Journal of the American
Geriatrics Society, vol. 63, no. 11, pp. 2415–2416, 2015.doi:10.1111/jgs.13804.

## 108
[74]    L. A. Lipsitz, A. E. Tchalla, I. Iloputaife,et  al., “Evaluation of an automated falls
detection  device  in  nursing  home  residents,”Journal  of  the  American  Geriatrics
Society, vol. 64, no. 2, pp. 365–368, 2016.doi:10.1111/jgs.13708.
[75]    E. Ruben, M. Francisco, J. Castro,et al., “A cross-dataset deep learning-based clas-
sifier for people fall detection and identification,”Computer Methods and Programs
in Biomedicine, vol. 184, pp. 0169–2607, 2020.
[76]    A. Nait, G. Englebienne, V. Schooten, M. Pijnappels, and B. Kr ̈os, “Deep learning to
predict falls in older adults based on daily-life trunk accelerometry,”Sensors, vol. 18,
p. 1654, 2018.
[77]    G. Tang, M. Muller, A. R. Gonzales, and R. Sennrich, “Why self-attention? a tar-
geted evaluation of neural machine translation architectures,”Association for Com-
putational Linguistics, 2018.
[78]    T. Theodoridis, V. Solachidis, N. Vretos, and P. Daras, “Human fall detection from
acceleration measurements using a recurrent neural network,”Information Technolo-
gies Institute, Centre for Research and Technology Hellas, 2017.
[79]    K.-C. Liu, C.-Y. Hsieh, H.-Y. Huang, S. J.-P. Hsu, and C.-T. Chan, “An analysis of
segmentation approaches and window sizes in wearable-based critical fall detection
systems with machine learning models,”IEEE  Sensors  Journal, vol. 20, pp. 3303–
## 3313, 2020.
[80]    O. Ojetola, E. Gaura, and J. Brusey, “Data set for fall events and daily activities
from inertial sensors,”Proceedings of the 6th ACM Multimedia Systems Conference,
pp. 243–248, 2015.
[81]    A. Sucerquia, J. Lopez, and J. Vargas-Bonilla, “Sisfall: A fall and movement dataset,”
Sensors (Basel), vol. 17(1), p. 198, 2017.
[82]    Noraxon.  “Myoresearch  3.14  user  manual,”  Noraxon  U.S.A.  Inc.  (2019),  [Online].
## Available:https://www.noraxon.com/noraxon-download/myoresearch3-14-
user-manual/.
[83]    San Diego State University,Neuromechanics and neuroplasticity laboratory-ens 216,
https://ens.sdsu.edu/dpt/research/faculty-research-interests/neuromechanics-
and-neuroplasticity-lab/, Accessed: Apr. 29, 2020, 2018.
[84]    K.  Frank,  M.  J.  Vera  Nadales,  P.  Robertson,  and  T.  Pfeifer,  “Bayesian  recogni-
tion  of  motion  related  activities  with  inertial  sensors,”  inProceedings  of  the  12th
ACM International Conference Adjunct Papers on Ubiquitous Computing - Adjunct,
Copenhagen,  Denmark:  Association  for  Computing  Machinery,  2010,  pp.  445–446.
[Online]. Available:https://doi.org/10.1145/1864431.1864480.

## 109
[85]    S. Gasparrini, E. Cippitelli, S. Spinsante, and E. Gambi, “A depth-based fall detection
system using a kinect®sensor,”Sensors, vol. 14, no. 2, pp. 2756–2775, 2014,issn:
1424-8220. [Online]. Available:https://www.mdpi.com/1424-8220/14/2/2756.
[86]    D. Micucci, M. Mobilio, and P. Napoletano, “Unimib shar: A new dataset for human
activity recognition using acceleration data from smartphones,”ArXiv, vol. abs/1611.07688,
## 2016.
[87]    J.  A.  Santoyo-Ram ́on,  E.  Casilari-P ́erez,  and  J.  M.  Cano-Garc ́ıa,  “Analysis  of  a
smartphone-based  architecture  with  multiple  mobility  sensors  for  fall  detection,”
PLoS ONE, vol. 11, 2018.
[88]    M. Kepski. “UR fall detection dataset.” (2014), [Online]. Available:http://fenix.
univ.rzeszow.pl/
## ~
mkepski/ds/uf.html.
[89]    X.  Yang,  F.  Xiong,  Y.  Shao,  and  Q.  Niu,  “Wmfall:  Wifi-based  multistage  fall  de-
tection with channel state information,”International Journal of Distributed Sensor
Networks, vol. 14, Oct. 2018.doi:10.1177/1550147718805718.
[90]    S. Aksoy and R. M. Haralick, “Feature normalization and likelihood-based similarity
measures for image retrieval,”Pattern Recogn. Lett., vol. 22, no. 5, pp. 563–582, Apr.
2001,issn: 0167-8655.doi:10.1016/S0167-8655(00)00112-4. [Online]. Available:
https://doi.org/10.1016/S0167-8655(00)00112-4.
[91]    S. Ioffe and C. Szegedy, “Batch normalization: Accelerating deep network training
by reducing internal covariate shift,”International conference on machine learning,
vol. 37, pp. 448–456, 2015.
[92]    O. Aziz, C. M. Russell, E. J. Park, and S. N. Robinovitch, “The effect of window size
and lead time on pre-impact fall detection accuracy using support vector machine
analysis of waist mounted inertial sensor data,” in2014  36th  Annual  International
Conference  of  the  IEEE  Engineering  in  Medicine  and  Biology  Society, IEEE, 2014,
pp. 30–33.
[93]    H. Fakhrulddin, X. Fei, and H. Li, “Convolutional neural networks (cnn) based human
fall detection on body sensor networks (BSN) sensor data,”International Conference
on Systems and Informatics (ICSAI), IEEE, pp. 1461–1465, 2017.
[94]    H.  Yhdego,  J.  Li,  C.  Paolini,  and  M.  Audette,  “Wearable  sensor  gait  analysis  of
fall detection using attention network,” in2021  IEEE  International  Conference  on
Bioinformatics and Biomedicine (BIBM), Houston, TX, USA, 2021.doi:10.1109/
## BIBM52615.2021.9669795.
[95]    J. Brownlee,Imbalanced Classification with Python: Better Metrics, Balance Skewed
Classes, Cost-Sensitive Learning. 2020.

## 110
[96]    R. Mohammed, J. Rawashdeh, and M. Abdullah, “Machine learning with oversam-
pling and undersampling techniques: Overview study and experimental results,” in
2020  11th  International  Conference  on  Information  and  Communication  Systems
(ICICS), 2020, pp. 243–248.doi:10.1109/ICICS49469.2020.239556.
[97]    S.-J. Yen and Y.-S. Lee, “Under-sampling approaches for improving prediction of the
minority class in an imbalanced dataset,” inIntelligent Control and Automation: In-
ternational  Conference  on  Intelligent  Computing,  ICIC  2006  Kunming,  China,  Au-
gust  16–19,  2006,  D.-S.  Huang,  K.  Li,  and  G.  W.  Irwin,  Eds.  Berlin,  Heidelberg:
Springer Berlin Heidelberg, 2006, pp. 731–740,isbn: 978-3-540-37256-1.
[98]    V.  Chawla,  W.  Bowyer,  O.  Hall,  and W.  Kegelmeyer,  “Smote:  Synthetic minority
over-sampling technique,”J. Artif. Int. Res., vol. 16, pp. 321–357, 2002.
[99]    C. Shorten and T. M. Khoshgoftaar, “A survey on image data augmentation for deep
learning,”Journal of Big Data, vol. 6, 2019.
[100]    T. Taewoong, F. Pfister, D. Pichler,et  al., “Data augmentation of wearable sensor
data for parkinson’s disease monitoring using convolutional neural networks,”ICMI,
## DOI:10.1145/3136755.3136817, 2017.
[101]    H. Zhang, M. Ciss ́e, Y. Dauphin, and D. Lopez-Paz, “Mixup: Beyond empirical risk
minimization,”ArXiv, vol. abs/1710.09412, 2017.
[102]    S. Yun, D. Han, S. J. Oh, S. Chun, J. Choe, and Y. J. Yoo, “Cutmix: Regulariza-
tion strategy to train strong classifiers with localizable features,”2019  IEEE/CVF
International Conference on Computer Vision (ICCV), pp. 6022–6031, 2019.
[103]    T.-Y.  Lin,  P.  Goyal,  R.  Girshickand,  K.  He,  and  P.  Dollar,  “Focal  loss  for  dense
object detection,”IEEE Transactions on Pattern Analysis and Machine Intelligence,
vol. 42(2), pp. 318–327, 2020.
[104]    S. Chernbumroong, S. Cang, and H. Yu, “Genetic algorithm-based classifiers fusion
for multisensor activity recognition of elderly people,”IEEE  Journal  of  Biomedical
and Health Informatics, vol. 19, pp. 282–289, 2015.
[105]    K. Lorincz, B.-r. Chen, G. W. Challen,et al., “Mercury: A wearable sensor network
platform  for  high-fidelity  motion  analysis,”  inACM  International  Conference  on
## Embedded Networked Sensor Systems, 2009.
[106]    T. A. N. Gia, V. K. Sarker, I. Tcarenko,et al., “Energy efficient wearable sensor node
for iot-based fall detection systems,”Microprocess. Microsystems, vol. 56, pp. 34–46,
## 2018.
[107]    M. Oquab, L. Bottou, I. Laptev, and J.Sivic, “Learning and transferring mid-level
image representations using convolutional neural networks.,”CVPR, 2014.

## 111
[108]    M. Dixit, Y. Li, and N. Vasconcelos, “Semantic fisher scores for task transfer: Using
objects  to  classify  scenes,”IEEE  Transactions  on  Pattern  Analysis  and  Machine
Intelligence,  vol.  42,  no.  12,  pp.  3102–3118,  2020.doi:10 . 1109 / TPAMI . 2019 .
## 2921960.
[109]    O. Rippel, J. Snoek, and R. P. Adams, “Spectral representations for convolutional
neural networks,” inAdvances in Neural Information Processing Systems, C. Cortes,
N. Lawrence, D. Lee, M. Sugiyama, and R. Garnett, Eds., vol. 28, Curran Associates,
Inc., 2015. [Online]. Available:https://proceedings.neurips.cc/paper_files/
paper/2015/file/536a76f94cf7535158f66cfbd4b113b6-Paper.pdf.
[110]    D. Zeiler and R. Fergus, “Visualizing and understanding convolutional networks.,”
CoRR abs/1311.2901, 2013.
[111]    X. Michael, J. Neal, B. Marshall, L. David, and E. Stefano, “Transfer learning from
deep features for remote sensing and poverty mapping.,”Department  of  Computer
Science and Earth system science, Stanford University, 2015.
[112]    A. Trivedi. “Transfer learning and fine-tuning deep convolutional neural networks.”
(2016),  [Online].  Available:http://blog.revolutionanalytics.com/2016/08/
deep-learning-part-2.html.
[113]    S. J. Pan and Q. Yang, “A survey on transfer learning,”Knowledge and Data Engi-
neering,IEEE Transactions on 22(10):1345–1359, 2010.
[114]    Y.-H. Byeon, S. B. Pan, and K.-C. Kwak, “Intelligent deep models based on scalo-
grams  of  electrocardiogram  signals  for  biometrics,”Sensors  (Basel,  Switzerland),
vol. 19, 2019.
[115]    Z. Wang and T. Oates, “Encoding time series as images for visual inspection and
classification using tiled convolutional neural networks,” 2014.
[116]    Z. Wang and T. Oates, “Imaging time-series to improve classification and imputa-
tion,” inInternational Joint Conference on Artificial Intelligence, 2015.
[117]    J. Faouzi and H. Janati, “Pyts: A python package for time series classification,”Jour-
nal of Machine Learning Research, vol. 21, no. 46, pp. 1–6, 2020. [Online]. Available:
http://jmlr.org/papers/v21/19-763.html.
[118]    J.-P.  Eckmann,  S.  O.  Kamphorst,  and  D.  Ruelle,  “Recurrence  plots  of  dynamical
systems,”Europhysics Letters, vol. 4, no. 9, p. 973, Nov. 1987.
[119]    N. Marwan, J. F. Donges, Y. Zou, R. V. Donner, and J. Kurths, “Complex network
approach for recurrence analysis of time series,”Physics Letters A, vol. 373, no. 46,
pp. 4246–4254, Nov. 2009.

## 112
[120]    J. Donahue, Y. Jia, O. Vinyals,et  al., “A deep convolutional activation feature for
generic visual recognition.,”DeCAF: arXiv:1310.1531, 2013.
[121]    K. He, X. Zhang, S. Ren, and J. Sun, “Deep residual learning for image recognition,”
CoRR, vol. abs/1512.03385, 2015.
[122]    R. Zeng, J. Wu, Z. Shao, L. Senhadji, and H. Shu, “Quaternion softmax classifier,”
Electronics letters, vol. 50, no. 25, pp. 1929–1931, 2014.
[123]    F. Zaman, “What is tensorflow lite,” 2020.
[124]    K. He, X. Zhang, S. Ren, and J. Sun, “Deep residual learning for image recognition,”
IEEE conference on computer vision and pattern recognition, pp. 770–778, 2016.
[125]    Z. Wang, W. Yan, and T. Oates, “Time series classification from scratch with deep
neural  networks:  A  strong  baseline,”International  joint  conference  on  neural  net-
works, pp. 1578–1585, 2017.
[126]    F. Karim, S. Majumdar, H. Darabi, and S. Chen, “Lstm fully convolutional networks
for time series classification,”IEEE Access, vol. 6, pp. 1662–1669, 2018.
[127]    T.  She,  X.  Sun,  Z.  Xia,  L.  Chen,  and  J.  Liu,  “Fall  detection  algorithm  based  on
triaxle accelerometer and magnetometer,”The  Journal  of  Mobile  User  Experience,
## Engineering Letters, 2016.
[128]    D. Jacob, C. Ming, K. Lee, and K. Toutanova, “BERT: Pre-training of deep bidirec-
tional transformers for language understanding,”NAACL, 2019.
[129]    Q. Chen, Z. Zhuo, and W. Wang, “Bert for joint intent classification and slot filling,”
Computing Research Repository (CoRR), vol. abs/1902.10909, 2019.
[130]    J. Cheng, L. Dong, and M. Lapata, “Long short-term memory-networks for machine
reading,”EMNLP, 2016.
[131]    A. Zhang, Z. C. Lipton, M. Li, and A. J. Smola, “Dive into deep learning,”https:
// d2l. ai, Release 0.7.0. 2020.
[132]    S. M. Kazemi, R. Goel, S. Eghbali,et al., “Time2vec: Learning a vector representation
of time,”ArXiv, vol. abs/1907.05321, 2019.
[133]    D. Hendrycks and K. Gimpel, “Gaussian error linear units (gelus),”arXiv: Learning,
## 2016.
[134]    J. Kim and N. Moon, “Bilstm model based on multivariate time series data in multiple
field for forecasting trading area,”Journal  of  Ambient  Intelligence  and  Humanized
## Computing, 2019.

## 113
[135]    A.  Graves,  A.-r.  Mohamed,  and  G.  E.  Hinton,  “Speech  recognition  with  deep  re-
current neural networks,”2013 IEEE International Conference on Acoustics, Speech
and Signal Processing, pp. 6645–6649, 2013.
[136]    A. N. Aicha, G. Englebienne, K. van Schooten, M. Pijnappels, and B. Kr ̈ose, “Deep
learning  to  predict  falls  in  older  adults  based  on  daily-life  trunk  accelerometry,”
Sensors, vol. 18, no. 5, p. 1654, 2018.doi:10.3390/s18051654.
[137]    B. Kwoleka and M. Kepski, “Human fall detection on embedded platform using depth
maps and wireless accelerometer,”computer  methods  and  programs  in  biomedicine,
vol. 117, pp. 489–501, 2014.
[138]    J. Vig and K. Ramea, “Comparison of transfer-learning approaches for response se-
lection in multi-turn conversations,” inAssociation for the Advancement of Artificial
Intelligence, Honolulu, HI,USA, 2019.
[139]    J. Devlin, M.-W. Chang, K. Lee, and K. Toutanova, “Bert: Pre-training of deep bidi-
rectional transformers for language understanding,” inNAACL-HLT, Minneapolis,
## MN, USA, 2019.
[140]    R.  Hadsell,  S.  Chopra,  and  Y.  LeCun,  “Dimensionality  reduction  by  learning  an
invariant mapping,”2006  IEEE  Computer  Society  Conference  on  Computer  Vision
and Pattern Recognition (CVPR’06), vol. 2, pp. 1735–1742, 2006.
[141]    E. Breck, N. Polyzotis, S. Roy, S. E. Whang, and M. A. Zinkevich, “Data validation
for machine learning,” inConference on Machine Learning and Systems, 2019.
[142]    A. Paszke, S. Gross, F. Massa,et al., “Pytorch: An imperative style, high-performance
deep  learning  library,”  inAdvances  in  Neural  Information  Processing  Systems  32,
Curran Associates, Inc., 2019, pp. 8024–8035. [Online]. Available:http://papers.
neurips.cc/paper/9015-pytorch-an-imperative-style-high-performance-
deep-learning-library.pdf.
## [143]    E. Casilari, M.
## ́
Alvarez-Marco, and F. Garcıa-Lagos, “A study of the use of gyroscope
measurements in wearable fall detection systems,”Symmetry, vol. 12, no. 4, p. 649,
## 2020.doi:10.3390/sym12040649.
[144]    J. Demsar, “Statistical comparisons of classifiers over multiple data sets.,”Journal
of Machine Learning Research, vol. 7, 2006.
[145]    V. A. Moskalenko, N. Y. Zolotykh, and G. Osipov, “Deep learning for ecg segmen-
tation,”ArXiv, vol. abs/2001.04689, 2020.
[146]    C. de Boor, “A practical guide to splines,” inApplied Mathematical Sciences, 1978.
[147]    J. A. Shaver, “Cardiac auscultation: A cost-effective diagnostic skill.,”Current prob-
lems in cardiology, vol. 20 7, pp. 441–530, 1995.

## 114
[148]    R. Ostfeld, Y. Goldberg, G. Janis, S. Bobra, H. Polotsky, and S. Silbiger, “Cardiac
auscultatory training among third-year medical students during their medicine clerk-
ship,” English (US),International Journal of Cardiology, vol. 144, no. 1, pp. 147–149,
## Sep. 2010,issn: 0167-5273.doi:10.1016/j.ijcard.2008.12.201.
[149]    J.  M.  Vukanovic-Criley,  A.  Hovanesyan,  S.  R.  Criley,et  al.,  “Confidential  testing
of  cardiac  examination  competency  in  cardiology  and  noncardiology  faculty  and
trainees: A multicenter study,”Clinical cardiology, vol. 33, no. 12, pp. 738–745, Dec.
## 2010,issn: 0160-9289.doi:10.1002/clc.20851.
[150]    J.  M.  Vukanovic-Criley,  S.  Criley,  C.  M.  Warde,et  al.,  “Competency  in  Cardiac
Examination Skills in Medical Students, Trainees, Physicians, and Faculty: A Mul-
ticenter  Study,”Archives  of  Internal  Medicine,  vol.  166,  no.  6,  pp.  610–616,  Mar.
## 2006.
[151]    S. Mangione, L. Z. Nieman, E. K. Gracely, and D. Kaye, “The teaching and practice of
cardiac auscultation during internal medicine and cardiology training: A nationwide
survey,”Annals of Internal Medicine, vol. 119, pp. 47–54, 1993.
[152]    S.  K.  March,  J.  L.  Bedynek,  and  M.  A.  Chizner,  “Teaching  cardiac  auscultation:
Effectiveness of a patient-centered teaching conference on improving cardiac auscul-
tatory skills.,”Mayo Clinic proceedings, vol. 80 11, pp. 1443–8, 2005.
[153]    J.  Pan  and  W.  J.  Tompkins,  “A  real-time  qrs  detection  algorithm,”IEEE  Trans-
actions  on  Biomedical  Engineering,  vol.  BME-32,  no.  3,  pp.  230–236,  1985.doi:
## 10.1109/TBME.1985.325532.
[154]    F. J. Ordonez and D. Roggen, “Deep convolutional and lstm recurrent neural net-
works for multimodal wearable activity recognition,”Sensors, vol. 16, no. 1, 2016,
issn: 1424-8220.doi:10.3390/s16010115. [Online]. Available:https://www.mdpi.
com/1424-8220/16/1/115.
[155]    F.  Cholletet  al.“Keras.”  (2015),  [Online].  Available:https : / / github . com /
fchollet/keras.
[156]    T. O’Malley, E. Bursztein, J. Long, F. Chollet, H. Jin, L. Invernizzi,et  al.,Keras-
tuner,https://github.com/keras-team/keras-tuner, 2019.
[157]    C. Tunca, G. Salur, and C. Ersoy, “Deep learning for fall risk assessment with iner-
tial sensors: Utilizing domain knowledge in spatio-temporal gait parameters,”IEEE
journal of biomedical and health informatics, vol. 24, Dec. 2019.doi:10.1109/JBHI.
## 2019.2958879.
[158]    J. Pel ́aez, J. Do ̃na, J. Fornari, and G. Serra, “Ischemia classification via ecg using
mlp neural networks,”International Journal of Computational Intelligence Systems,

## 115
vol. 7, pp. 344–352, 2 2014,issn: 1875-6883.doi:10.1080/18756891.2014.889498.
[Online]. Available:https://doi.org/10.1080/18756891.2014.889498.
[159]    H.  Yhdego,  J.  Li,  S.  Morrison,et  al.,  “Towards  musculoskeletal  simulation-aware
fall  injury  mitigation:  Transfer  learning  with  deep  cnn  for  fall  detection,”Spring
Simulation Conference (SpringSim),IEEE, pp. 1–12, 2019.
[160]    D. P. Kingma and J. Ba,Adam: A method for stochastic optimization, 2014.
[161]    B. Bengfort and R. Bilbro, “Yellowbrick: Visualizing the scikit-learn model selection
process,”Journal of Open Source Software, vol. 4, no. 35, p. 1075, 2019.
[162]    M. Abadi, P. Barham, J. Chen,et al.,Tensorflow: A system for large-scale machine
learning, 2016.
[163]    L. Li, K. Jamieson, G. DeSalvo, A. Rostamizadeh, and A. Talwalkar, “Hyperband:
A novel bandit-based approach to hyperparameter optimization,” 2016.

## 116
## APPENDIX A
## REAL-TIME ECG-BASED VIRTUAL PATHOLOGY
## STETHOSCOPE DETECTION
Cardiac  auscultation  is  the  auditory  detection  of  heart  sounds  to  diagnose  abnor-
malities,  a  crucial  skill  that  is  both  efficient  and  cost-effective  in  medical  practice[147].
However, due to the increased prevalence and usage of expensive cardiac technologies, such
as  echocardiography,  many  new  physicians  and  trainees  have  difficulty  performing  essen-
tial  cardiovascular  examinations  on  their  patients,  particularly  diagnosing  abnormalities
through auscultation using a stethoscope [148]–[152].  Because the stethoscope is an inex-
pensive  technology  that  relies  on  the  physician  to  locate  auscultation  sites  and  diagnose
common structural cardiac conditions properly, trainees should focus on perfecting this skill
rather than turning to cost-inefficient technologies that serve a similar function.  With in-
creasingly shorter hospital visits, trainees do not have enough opportunities to practice on
actual patients in a clinical setting [150].  Alternative teaching methods must be developed
to educate on proper auscultation.
We extend the previous findings by considering patient position (seated versus supine),
obesity, and gender as factors that may influence accuracy.  In addition, site identification
can only occur after signal data is received and processed through low-pass and high-pass
filtering.  The algorithm used to identify each auscultation site must be trained on SP ECG
data to introduce real-time detection.  Training the neural network models using real SP
data will allow for both immediate and more accurate site detection.
This study reports the accuracy of a newly developed stethoscope simulation with
reduced signal collection noise on 41 SPs and the development of trained neural network
models  on  those  SPs  to  provide  a  real-time  element  to  the  ECG-based  stethoscope.   In
addition,  heart  sound  amplitudes  were  recorded  at  each  of  the  four  sites  to  determine  if
minute variations exist that can act as an additional classifier to the ECG data.  A second
data classifier would improve the stethoscope’s accuracy and the rapidity of signal processing
for identification.
Using standardized patients, individuals trained to portray patients have become a
common practice. SPs provide a safe and reliable setting for training clinical skills. However,
the range of symptoms and syndromes they can physically portray stays limited, since they
are typically healthy patient actors.  Augmenting SPs with the ability to simulate numerous
abnormalities would enhance the experience and variety of symptoms medical students can
encounter.
This study uses a wearable sensor that generates ECG signals to accurately recognize
the stethoscope position.  Our goal is to improve the accuracy and robustness of stethoscope
detection by using ECG sensor signals and deep-learning algorithms.  The contributions of
this article include the following:

## 117
•To the best of our knowledge, we contribute the first approach for virtual pathology
stethoscope detection simulation using various deep learning architectures.
•Validating  and  testing  the  findings  using  k-fold  and  leave-one-out  cross-validation
methods on supine and seated datasets.  Several experiments were conducted to deter-
mine the effectiveness of combining supine and seated datasets and testing at different
levels of obesity across different genders.  A real-time setting is also applied for detect-
ing stethoscopes by running live data (testing every one beat or 1 second) to improve
the model continuously.
Fig. 28.  Data processing pipeline for ECG-based stethoscope detection
## A.1 Methodology
This section presents the details of data acquisition, wearable sensor equipment, and
the  proposed  ECG-based  stethoscope  detection  methodology.   Electrocardiograms  record
the electrical signals in the heart.
First, as ECG contains many unwanted noises, preprocessing and filtering the raw
data are critical for ECG classification efficiency.  Second, segmentation is performed on the

## 118
input to extract the characteristic QRS and T wave.  The QRS complex represents ventric-
ular depolarization of the heart and consists of Q wave, R wave, and S wave components.
Third, to take advantage of both features, we propose a feature concatenation concept to
improve  the  identification  accuracy  of  the  four  positions  of  the  stethoscope.   Lastly,  the
feature vectors extracted from the neural network architectures and the hand-crafted QRS
and  T  features  are  fused  and  given  as  input  to  the  Softmax  classifier.   The  overall  data
processing pipeline for our ECG-based stethoscope detection is demonstrated in Figure 28.
A.1.1 Subjects, Equipment and Data Acquisition
Informed consent was obtained from 41 recruited SPs in Eastern Virginia Medical
School’s Sentara Center for Simulation and Immersive Learning to conduct twice in-person
trials of the ECG-based detection stethoscope.  The SPs were chosen by availability among
50 previously tested in a pilot study.
The participants involved were not required to complete any prior training or eval-
uations of the appointment,  nor did they act out any symptoms of cardiac abnormalities
during data acquisition.  The use of SPs for this study was strictly for obtaining ECG signal
data and testing site detection’s accuracy and real-time capabilities.  Among the 19 males
and 22 females tested, the average age of SPs was 42 (in the range of 27-61), and the aver-
age body mass index was 26 (in the range of 23-39).  A WelchAllyn Meditron stethoscope
apparatus was used to collect 10 seconds of heart sounds from each of the four auscultation
sites.
Two  direct-contact  electrodes  fixed  on  a  standard  stethoscope  chestpiece  recorded
ECG  signals  from  the  four  auscultation  sites  of  an  SP’s  chest.   The  setup  consisted  of  a
double-lead configuration and an additional base lead placed on the SP’s leg.  The setup
is attached to a Raspberry Pi computer to run the ECG signal acquisition program.  The
recorded ECG signals were preprocessed and filtered.  Noises and artifacts from breathing,
body  movements,  and  power  line  interference  were  filtered  using  low-pass  and  high-pass
filters.  A Butterworth low pass filter with a 40Hz cut-off frequency removes the power line
noise.  Similarly, a high-pass FIR filter with a cut-off frequency of 0.7Hz is used to reduce
baseline wandering noises and low-frequency artifacts.
ECG signals are acquired during the initial appointment for each of the 41 SPs.  For
testing the stethoscope orientation,  data is collected using angular variations by rotating
the stethoscope chestpiece clockwise with increments of 45
## ◦
to produce a total of four ori-
entations for each auscultation site.  A total of 5 runs of acquired ECG data per angle at
each auscultation site, and each run consisted of 10 beats of signals.  Datasets were collected
from both seated and supine positions.
Ten SPs returned for a second appointment to assess the SP-specific trained algo-
rithm’s accuracy and speed.  Using the same stethoscope apparatus as the initial appoint-
ments, a total of five runs of ECG data were collected per angle at each auscultation site,
both seated and supine.  Rather than collecting 10 beats of signals, the program automati-
cally stopped collecting data after approximately one beat, providing a site prediction and
assessing the accuracy of the prediction.

## 119
TABLE 16.  Characteristics of the Standardized Patients.
## Characteristics
## Subjects (n = 41)
Male, n19
Female, n22
## Age (years)43.7±17.2
## BMI26.75±4.9
Body fat(%)21.9±6.78
A.1.2 Segmentation and QRS Wave Feature Extraction
Feature  extraction  provides  fundamental  attributes  (amplitudes  and  intervals)  for
subsequent  ECG  classification.   After  pre-processing,  it  is  required  to  extract  features  of
the ECG signals in order to use them for the classifier.  The following section uses different
neural network methods to obtain deep features.  Several feature extraction neural network
configurations are compared, with the original multi-layer perceptron serving as a baseline
for  comparison.   However,  the  features  extracted  using  the  neural  network  may  not  fully
capture  the  subtle  difference  between  the  CA  areas  on  different  subjects.   In  our  second
approach, these automatically extracted features using neural networks from the raw ECG
data  concatenated  with  hand-crafted  features.   We  segmented  the  raw  signal  into  a  one-
second window equally centered on each R-peak using an RR interval.  The ECG signal was
down-sampled to 200Hz (or 200 samples per second).
For the hand-crafted features, we adapt the Pan-Tompkins algorithm [153] to identify
the well-recognized Q, R, and S peaks along with their corresponding time indices.  R-peak
is  identified  first,  followed  by  Q  and  S.  After  finding  the  location  of  the  R  peaks  of  the
signal, we identify the Q and S peaks by local minimums to a presented sample threshold.
The minimum Q and S peaks are searched in the range of -5 to -20 and 1 to 25 samples
from the R-peak.  A sample occurs every 5 ms, as the sampling frequency is 200 Hz.  Next,
using a wave segmentation method [154], the T wave peak was found by searching for local
maxima within a pre-defined distance from the R peak.  Amplitude and interval features
are then extracted from the onset, peak, and offset of the QRS and T wave as illustrated in
## Fig. 29.
## A.1.3 Deep Learning Methods
This section presents different types of neural network approaches selected for our
work.  We tested six deep-learning architectures to provide a fully comprehensive baseline
for ECG-based stethoscope detection.  It covers the old feed-forward network to the most
recent state-of-the-art ones for sequential data.  The input data for all the models are shaped
to fit the 3D input shape of Keras’s [155] sequential model.  The 3D input shape would be
sequence length, batch size, and the number of features.  The sequence length of the inputs
is  1s  window  signal-200  samples,  and  the  batch  size  for  all  the  models  is  100  windows.
The epoch size of 50 provides the best results.  Hence,  the input shape for the models is

## 120
Fig. 29.  Segmented ECG data from the Mitral area.  Amplitude and interval features are
extracted from the onset, peak, and offset of QRS and T waves.
Fig. 30.  MLP Model Architecture.
200x100x1, where our number of features is 1.  We have total windows of 32,800, which is
10 sec x 5 times recorded x 4 positions x 4 orientation of sensors x 41 subjects.
Multi-layer Perceptron (MLP)
MLP, a foundation architecture for deep learning methods,  is a feed-forward network for
the supervised learning approach.  MLP is a fully connected layer (dense layer) consisting
of an input layer that accepts the input data, an output layer that provides the prediction
classes, and one or more hidden layers between those two layers where all the computation
is done.  We used an MLP with three hidden layers, and the rectified linear (ReLU) func-
tion  is  an  activation  function  in  the  hidden  units.   To  train  the  network,  we  used  scaled
conjugate gradient back-propagation.  We are standardizing the data before training,  im-
proving the performance accuracy.  To optimize the hyper-parameters of the MLP model,
we use KerasTuner [156].  We evaluated different hidden layers and hidden units on each
layer in the range of 1-5 and 3-30 [157], [158], respectively.  The final model, as shown in
Figure  30  contains  three  dense  layers  (fully  connected)  followed  by  ReLU  activation  and
dropout regularization.  The concatenated vector of QRS and T-wave features and features

## 121
Fig. 31.  CNN model Architecture.
processed using the MLP are fed to Softmax’s output layer.  Lastly, the Softmax function
layer  converts  the  concatenated  vector  into  estimating  probabilities  for  each  stethoscope
position.
Convolution Neural Network (CNN)
Our CNN architecture, as shown in Figure 31 involves two hierarchical 1D CNN connected
in  series,  followed  by  a  pooling  layer  and  a  dropout  regularization  [159].   One-dimension
convolution is performed over the time dimension by the convolutional kernel.  Stacking the
CNN layers in two sets provides a higher chance of learning the feature from the input ECG
signals.   A  dropout  layer  is  included  to  slow  down  the  learning  process  for  a  better  final
model.  The pooling layer reduces the learned features and consolidates them into essential
elements.  Finally, the learned features are flattened to one long vector and pass through a
fully connected layer for CA prediction.  The fully connected layer ideally provides a buffer
between the learned features and feeds its output to the final classification layer (Softmax
Layer) that provides the four classes.  Each cell has an inner dimension of 128 parallel feature
maps and a kernel size of 5.  The initial feature extraction from the input data is performed
in the fully connected layer, while a second fully connected layer collects the output from
the convolutional to the final classification layer [159].
Long Short Term Memory (LSTM)
We experimented with different models of LSTM layers and the use of dense layers using
KerasTuner[155]  hyper-parameter  optimization  framework.   We  also  analyzed  the  use  of
dropout  layers  next  to  the  LSTM  layers  to  prevent  over-fitting  problems.   Based  on  the
hyper-parameter search, as shown in Figure 32, we use a model with a stack of two LSTM
layers followed by dropout and two dense and Softmax layers at the end of the network.
The first LSTM layer maps from many input signals into many output feature vectors.  In
contrast, the second LSTM layer maps the many input feature vectors from the previous
layer into a single output feature vector.  The number of hidden units for each layer equals
the size of the time windows.  Moreover, the number of epochs for training the model was
explored using the hyper-parameter optimization in the range of 50-500 [157], and we got
better performance with 50 epochs.  We shuffle our dataset at each epoch to reduce over-
fitting  and  variance  caused  by  training  dataset  order.   Adam  optimizer  [160]  is  used  for
training the model.
## CNN-LSTM
The architecture of a hybrid CNN-LSTM, similar to the model proposed by[154],  is used
for this method.  We stack two conv1D-ReLU-pooling, followed by dropout, one LSTM, and
two dense and Softmax layers as shown in Figure 33.  After examining the number of layers

## 122
Fig. 32.  LSTM Model Architecture.
Fig. 33.  CNN-LSTM Model Architecture.
for conv1D and LSTM, we figured out that increasing the number for LSTM didn’t improve
the accuracy metrics.  The stacked conv1D block results in the same dimension time series
vector with the number of neurons in the conv1D layer.  Each slice along the time dimension
of the output of the stacked conv1D blocks is fed to one LSTM unit.  The number of LSTM
units is equal to the time window of the output of the stacked conv1D block.  The LSTM
layer accepts many feature vectors get from the conv1D layer and then outputs one feature
vector to be fed to the dense layer.
## Transformer
We  use  a  simple  transformer  encoder  model  as  shown  in  Figure  34,  with  stacks  of  two
attention  neural  network  blocks  [12].   Contrary  to  the  natural  language  processing(NLP)
model,  we  do  not  need  to  decode  layers  for  such  applications.   This  transformer  model
contains time embedding and two dense layers in addition to the two stacks of attention
neural networks.  The two similar attention network layers have two sublayers- a multi-head
self-attention with layer normalization and a feed-forward neural network.
Attention neural networks need to embed the time to our sequential input data.  NLP
models use sin-cosine sinusoidal functions for positional encoding.  However, as our input
time series has scalar values than distinct words in NLP, we have to encode the sequence
of time which is hidden in our signal data [94].  We use and implement the existing method
that helps us to learn the vector representation of time- Time2Vec for time embedding [132].
Our input to the models is 3D sequential data, where the dimensions are the feature
size (which is one in our case), batch size of sequences (number of windows), and sequence
length (200).  We get a vector representation output from the transformer encoder model
for each input sequence length.

## 123
## Fig. 34.  Transformer Model Architecture.
## A.1.4 Performance Evaluation
Performance evaluation using 10-fold cross-validation (sample-based accuracy) and
leave-one-out subject cross-validation (subject-based accuracy) are used for comparing the
models.  In k-fold cross-validation,  a sub-sample of the test subjects may be used for the
training, which might slightly increase the performance of the different methods.  Whereas,
in leave-one-out subject cross-validation, a subject that has never been trained on the subject
before is used for testing.  Hence, we also evaluate our performance results using leave-one-
out subject cross-validation.  Leave-one-out subject cross-validation helps us ensure we do
not  have  subject  bias.   The  leave-one-out  subject  method  is  performed  using  one  of  the
subjects from the datasets for testing and the rest for training.  We repeated the process for
all 41 subjects.
We  used  accuracy  (both  sample-based  and  subject-based)  as  our  primary  perfor-
mance evaluation metric, which is the ratio of the number of correct predictions to the total
number of input samples or subjects, as shown in the formula below.  The accuracy result
is calculated and averaged for all the test data across the cross-validation folds or subjects
to get a single accuracy value for each model.
## Accuracy=
## TP+TN
## TP+TN+FP+FN
Although our dataset is balanced, using accuracy only as performance evaluation metrics
may fail to capture some classification biases.  Hence, we also used receiver operating char-
acteristic curves (ROC) and presented each model’s area under the curve (AUC) values.  A
ROC-AUC helps us visualize the trade-off between the model’s sensitivity and specificity.
ROC-AUC curves are used mainly for binary classification.  As our problem is multi-class
classification,  we  use  yellow  bricks  [161]  ROC-AUC  visualizer.   It  provides  ways  for  vi-
sualizing multi-class classification curves by binarizing the output (per class).  We plot a
ROC-AUC  curve  and  its  AUC  score  for  each  of  our  four  classes.   This  analysis  helps  us
see the trade-off between sensitivity and specificity per class.  Moreover, we also report the
average of all class curves across all methods using the macro-average ROC-AUC curve.
AUC  is  calculated  as  the  area  under  theSensitivity(True  Positive  Rate)-(1−
Specificity)  (False  Positive  Rate)  curve,  where  sensitivity  and  specificity  are  calculated
as follows.
Sensitivity=Recall=
## TP
## TP+FN

## 124
## Specificity=
## TN
## FP+TN
All the deep learning (DL) models proposed and the results presented are implemented in
TensorFlow  [162]  with  Keras  [155].   We  also  use  hyperband  [163]  from  KerasTuner  [156]
framework for hyper-parameter searches like the number of layers, number of neurons and
number of epochs, and learning rate.  To optimize the network, the efficient Adam optimizer
[160]  is  used,  and  the  categorical  cross-entropy  loss  function  is  used,  given  that  we  are
learning  a  multi-class  classification  problem.   The  training  and  testing  experiments  were
executed on an Nvidia v100 GPU from ODU university’s virtual high-performance cluster.
A.2 Results and Discussion
This section reported the experiment results of the deep learning models proposed
above.   First,  we  discuss  the  accuracy  (sample-based  accuracy),  subject  accuracy,  ROC
curve, and AUC scores of the different models on the test data.  Then, compare the FCN
model for different standardized patient postures and input features.  Lastly,  we evaluate
the FCN model on various characteristics of standardized patients.
The ROC curve results and its AUC scores for sample-based and subject-based clas-
sifiers are presented in Figures 35 and 36, respectively.  As we can see from those figures and
the accuracy results in Table 17, the subject-based methods perform less than the sample-
based methods.  It is because, in sample-based methods, we can have the same subject in our
training and testing datasets.  Which makes the deep learning method know more about the
subject.  Whereas, in subject-based models, a brand-new subject, never used for training, is
utilized for testing.
The average sample-based accuracy of all tested methods is presented in Table 17.
MLP  performs  worse  than  the  other  methods  (accuracy  of  0.83).   In  contrast,  the  FCN
method outperforms the other approaches by a sensible margin (accuracy of 0.96 compared
to  the  nearest  accuracy  of  0.94  and  0.93  by  CNN-LSTM  and  Transformer,  respectively).
This result shows that FCN, which employs a convolution layer with a large receptive field
for the last fully connected layer, achieves much more performance than CNN, LSTM, and
combined (CNN-LSTM). CNN-LSTM and Transformer demonstrate similar performance in
all performance metrics, while they are better than LSTM, CNN, and MLP models.
The average subject-based accuracy and subject-based ROC-AUC curve of all the
models are presented in the second column in Table 17 and Figure 36.  FCN, Transformer,
and CNN-LSTM, in descending order, still have superior performance than the other meth-
ods.    LSTM,  a  sequence-label  classifier,  outperforms  CNN,  a  sample-label  classifier,  in
subject-based accuracy,  while their performance on the sample-based is almost the same.
This exhibits feeding the whole subject dataset sequence to the LSTM model helps us to
get better performance.
We trained the FCN model with the raw ECG data and QRS + T wave’s handcrafted
features and tested the model’s performance for seated, supine, and combined SP positions.
Tables  18  depict  the  model  performance  for  the  different  postures.   The  supine  position
outperforms  the  seated  position  in  all  the  performance  metrics.   For  the  supine  position,
we achieved an accuracy of 0.96 and 0.97 AUC score.  However, the performance was lower
in the seated position for all CA areas.  We obtained an accuracy of 0.93 and 0.94 AUC

## 125
(a) FCN(b) Transformer(c) CNN-LSTM
(d) LSTM(e) CNN(f) MLP
Fig. 35.  Sample-based ROC curves on test data.

## 126
(a) FCN(b) Transformer(c) CNN-LSTM
(d) LSTM(e) CNN(f) MLP
Fig. 36.  Subject-based ROC curves on test data.

## 127
TABLE 17.  Comparing the results of different DL models on test data.
DataaAverage Model Performance(%)
Sample AccuracySubject AccuracyAUC
## MLP0.830.700.85
## CNN0.880.810.91
## LSTM0.870.830.90
## CNN-LSTM0.920.850.94
## Transformer0.910.860.93
## FCN0.940.880.96
1 sec row data (sampled at 200 pts/sec) and its handcrafted QRS and T- wave features.
score for the seated position.  Although efforts were made to reduce noise from the data, we
observed higher irregularities (e.g., movement artifacts) on the most seated ECG data.  This
may contribute to the lower results, but more tests must be done.  To present and analyze
our different models’ results, we combined and shuffled the two postures dataset into one
dataset.
TABLE 18.  FCN Model performance for different SP postures
DataaAverage Model Performance(%)
Sample AccuracySubject AccuracyAUC
## Seated0.930.850.94
## Supine0.960.900.97
## Combined0.940.880.96
1-sec row data (sampled at 200 pts/sec) and its handcrafted QRS and T- wave features.
Hand-crafted  extracted  features  (QRS  and  T)  and  the  features  learned  by  neural
network algorithms significantly impact ECG-based stethoscope detection. Recognizing each
cardiac cycle in the ECG signal enables us to obtain a good performance of classification
of  events  that  occur  on  this  signal.   The  QRS  is  a  well-recognized  waveform  within  the
ECG and serves as the starting point for automated diagnosis and classification schemes.  In
all CA areas, the FCN model learned from both neural network features and hand-crafted
features (QRS + T wave features) achieved higher classification accuracy than the model
trained on the neural network only.
The  results  obtained  on  using  both  validations  (sample-based  and  subject-based)
methods  highlight  two  points:  using  both  neural  network  features  and  QRS  +  T  hand-
crafted features helps us to get better performance than using one of them, and the stability
of the FCN model which can obtain top-performing performance on both validation methods.

## 128
One interesting observation is that detecting the Aortic and Pulmonic areas is easier than
the Mitral and Tricuspid areas.  The sample-based and subject-based ROC-AUC curve score
for both the Aortic and Pulmonic has better performances than the other stethoscope areas.
TABLE 19.  Comparison of the FCN model using different input features
Data/FeaturesAverage Model Performance(%)
Sample AccuracySubject AccuracyAUC
## Raw Data0.860.820.89
Raw Data+Wavesa0.940.880.96
QRS and T- wave features.
TABLE 20.  Comparison of the FCN model on gender data
FeaturesaAverage Model Performance(%)
Sample AccuracySubject AccuracyAUC
## Male0.870.840.92
## Female0.890.850.93
1-sec row data (sampled at 200 pts/sec) and its handcrafted QRS and T- wave features.
Lastly, we compared the performance based on the characteristics of the standardized
patients.  As we can see in Table 20, the FCN model’s performance on the female datasets
is almost the same as the male datasets.
Furthermore, datasets of normal and overweight body mass index (BMI) performance
are compiled in Table 21.  But, the performances on both datasets have very close results,
which highlights the stability of this model, and suggests its good generalization capacity
on different datasets.  Site identification using ECG detection is not significantly influenced
by SPs’ position, obesity (BMI), and gender.  The stethoscope orientation on the SP chest,
which may differ for each trainee, is resolved by augmenting the datasets.
TABLE 21.  Comparison of the FCN model based on BMI Data (Normal vs Overweight)
FeaturesaAverage Model Performance(%)
Sample AccuracySubject AccuracyAUC
## Normal0.910.840.93
## Overweight0.890.830.91
1-sec row data (sampled at 200 pts/sec) and its handcrafted QRS and T- wave features.

## 129
## A.3 Conclusion
This  work  investigated  the  accuracy  and  validity  of  a  deep  learning-based  virtual
pathology  stethoscope  detection  system.   It  involved  collecting  and  examining  ECG  data
from multiple subjects.  The experimental analysis done on different subjects ensures good
performance.   The  result  demonstrate  that  FCN,  CNN-LSTM,  and  Transformer  encoder
methods (0.96,  94,  and 0.93 AUC score,  respectively) lead to better performance.  More-
over,  the leave-one-out subject-based validation,  as well as different postures,  helps us to
determine the reliability of the system.
In future work, we plan to incorporate audio-recorded signals with the ECG signal.
Furthermore,  sequentially classifying the signals must also be studied;  such a system will
perform real-time classification and predict the incoming beat or section of an ECG signal.
For instance, if the current signal is from a particular area, the next signal is more likely to
be from the same area.

## 130
## VITA
## Haben Girmay Yhdego
Department of ENGINEERING - MODELING AND SIMULATION
## Old Dominion University
Norfolk, VA 23529
Haben Yhdego is a graduate research assistant and Ph.D. Candidate in the Electrical
and Computer Engineering.  He earned his undergraduate in Information Technology from
Mekelle University, Mekelle, Ethiopia in 2012.  And He received MSc in Image processing
and computer vision from Jean-Monet University,  Saint-Etienne,  France in 2015.  His re-
search interests include applying machine learning methods for images and sensor datasets,
image analysis, and computer vision.  He published some journal and conference papers in
Simulation,  MDPI,  IEEE,  and  SpringSim-  related  to  the  application  of  machine  learning
methods to sensor signal datasets.  His email address is yhdego.haben@gmail.com.