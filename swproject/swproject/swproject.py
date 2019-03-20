import numpy as np
import cv2
from matplotlib import pyplot as plt

def HogDescriptor(image):
        hog = cv2.HOGDescriptor()
        hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
        (rects, weights) = hog.detectMultiScale(image, winStride=(5,5),padding=(16,16), scale=1.05, useMeanshiftGrouping=False)
        return rects 
def Haarcascade( frame):
    face_cascade = cv2.CascadeClassifier('haarcascade_frontface.xml')
    body_cascade = cv2.CascadeClassifier('haarcascade_upperbody.xml')
    faces = face_cascade.detectMultiScale(gray,1.3,5)
    bodies = body_cascade.detectMultiScale(gray,1.1,5)
    return faces,bodies
    
cap = cv2.VideoCapture(0,cv2.CAP_DSHOW)

while(1):
    ret,frame = cap.read()
    #frame = cv2.imread("test1.jpg",cv2.IMREAD_COLOR)
    gray = np.zeros(frame.shape)
    gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    #processing

    ###HoG###
    #rects = HogDescriptor(gray)
    #for (x,y,w,h) in rects:
    #    cv2.rectangle(frame,(x,y),(x+w,y+h),(255,0,0),2)

    ###Haar###
    faces,bodies = Haarcascade(gray)
    for (x,y,w,h) in faces:
        cv2.rectangle(frame,(x,y),(x+w,y+h),(255,0,0),2)
    for (x,y,w,h) in bodies:
        cv2.rectangle(frame,(x,y),(x+w,y+h),(0,255,0),2)

    cv2.imshow("captured",frame)
    if cv2.waitKey(1) == 99:
        break
    #cv2.waitKey(0)
cv2.destroyAllWindows()