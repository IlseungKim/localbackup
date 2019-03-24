import numpy as np
import cv2
from matplotlib import pyplot as plt

def Haarcascade( frame):
    face_cascade = cv2.CascadeClassifier('haarcascade_frontface.xml')
    body_cascade = cv2.CascadeClassifier('haarcascade_fullbody.xml')

    faces = np.array(face_cascade.detectMultiScale(gray,1.3,5,minSize=(15,40), maxSize=(300,400)))

    bodies = np.array(body_cascade.detectMultiScale(gray,1.03,5,minSize=(100,200), maxSize=(300,400)))
    bodies = np.append(bodies,body_cascade.detectMultiScale(gray,1.1,5,minSize=(100,200), maxSize=(300,400)))
    bodies = np.append(bodies,body_cascade.detectMultiScale(gray,1.3,5,minSize=(100,200), maxSize=(300,400)))

    faces = np.reshape(faces,(faces.size,1))
    bodies = np.reshape(bodies,(bodies.size,1))
    return faces,bodies
    
cap = cv2.VideoCapture(0,cv2.CAP_DSHOW)
ret,frame = cap.read()
print(frame.shape)

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
    for i in range(0,faces.size,4):
        x = int(faces[i])
        y = int(faces[i+1])
        w = int(faces[i+2])
        h = int(faces[i+3])
        cv2.rectangle(frame,(x,y),(x+w,y+h),(255,0,0),2)
    #for (x,y,w,h) in faces:
    #    cv2.rectangle(frame,(x,y),(x+w,y+h),(255,0,0),2)
    for i in range(0,bodies.size,4):
        x = int(bodies[i])
        y = int(bodies[i+1])
        w = int(bodies[i+2])
        h = int(bodies[i+3])
        cv2.rectangle(frame,(x,y),(x+w,y+h),(0,255,0),2)

    cv2.imshow("captured",frame)
    if cv2.waitKey(1) == 'q':
        print("End")
        break
    #cv2.waitKey(0)

cv2.destroyAllWindows()
