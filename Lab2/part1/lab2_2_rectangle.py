import cv2
import numpy as np
import math

img = np.zeros((512, 512, 3), np.uint8)
windowName = 'Lab2-2'
cv2.namedWindow(windowName)

# Store points for the line
control_points = []

def draw_rectangle(control_points):
    num_lines = len(control_points) - 1
    for nl in range(num_lines):
        P0, P1 = control_points[nl], control_points[nl+1]

        x1, y1 = P0
        x2, y2 = P1
        x = min(x1, x2)
        y = min(y1, y2)
        
        width = abs(x2 - x1)
        height = abs(y2 - y1)

        top_left = (x, y)
        top_right = (x + width, y)
        bottom_left = (x, y + height)
        bottom_right = (x + width, y + height)

        cv2.line(img, top_left, top_right, (0, 255, 0), 2)
        cv2.line(img, top_right, bottom_right, (0, 255, 0), 2)
        cv2.line(img, bottom_right, bottom_left, (0, 255, 0), 2)
        cv2.line(img, bottom_left, top_left, (0, 255, 0), 2)

        control_points.clear() # to separate the rectangles


def mouse_callback(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        cv2.circle(img, (x, y), 3, (0, 0, 255), -1)
        control_points.append((x, y))
        if len(control_points) >= 2:
            draw_rectangle(control_points)

cv2.setMouseCallback(windowName, mouse_callback)

while True:
    cv2.imshow(windowName, img)
    if cv2.waitKey(20) & 0xFF == 27:
        break

cv2.destroyAllWindows()
