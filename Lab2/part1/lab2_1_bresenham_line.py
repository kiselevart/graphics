import cv2
import numpy as np
import math

img = np.zeros((512, 512, 3), np.uint8)
windowName = 'Lab2-1'
cv2.namedWindow(windowName)

# Store points for the line
control_points = []

def draw_bresenham(control_points):
    for i in range(0, len(control_points) - 1, 2):
        P0, P1 = control_points[i], control_points[i+1]
        x0, y0 = P0
        x1, y1 = P1

        dx = abs(x1 - x0)
        dy = abs(y1 - y0)

        sx = 1 if x0 < x1 else -1
        sy = 1 if y0 < y1 else -1
        err = dx - dy

        while True:
            img[y0, x0] = (50, 168, 82)  

            if x0 == x1 and y0 == y1:
                break

            e2 = 2 * err

            if e2 > -dy:
                err -= dy
                x0 += sx

            if e2 < dx:
                err += dx
                y0 += sy

def mouse_callback(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        cv2.circle(img, (x, y), 3, (0, 0, 255), -1)
        control_points.append((x, y))
        if len(control_points) >= 2:
            draw_bresenham(control_points)

cv2.setMouseCallback(windowName, mouse_callback)

while True:
    cv2.imshow(windowName, img)
    if cv2.waitKey(20) & 0xFF == 27:
        break

cv2.destroyAllWindows()
