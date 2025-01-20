from keras.models import load_model  # TensorFlow is required for Keras to work
from PIL import Image, ImageOps  # Install pillow instead of PIL
import numpy as np
import cv2
import time
import serial

arduino = serial.Serial(port='COM5', baudrate=9600, timeout=.1) 

# Disable scientific notation for clarity
np.set_printoptions(suppress=True)

# Load the model
model = load_model("keras_model.h5", compile=False)

# Load the labels
class_names = open("labels.txt", "r").readlines()

# Set up webcam
cap = cv2.VideoCapture(0)  # 0 is usually the default webcam; use 1 or other index if needed

# Check if the webcam opened successfully
if not cap.isOpened():
    print("Error: Could not open webcam.")
    exit()

# Define the size and initialize data array for predictions
size = (224, 224)
data = np.ndarray(shape=(1, 224, 224, 3), dtype=np.float32)

try:
    while True:
        # Capture frame-by-frame
        ret, frame = cap.read()
        if not ret:
            print("Failed to grab frame from webcam.")
            break

        # Convert the frame to a PIL image and resize
        image = Image.fromarray(cv2.cvtColor(frame, cv2.COLOR_BGR2RGB))
        image = ImageOps.fit(image, size, Image.Resampling.LANCZOS)

        # Turn the image into a numpy array and normalize it
        image_array = np.asarray(image)
        normalized_image_array = (image_array.astype(np.float32) / 127.5) - 1
        data[0] = normalized_image_array

        # Make a prediction
        prediction = model.predict(data)
        index = np.argmax(prediction)
        class_name = class_names[index]
        confidence_score = prediction[0][index]

        # Display the results
        print("Class:", class_name.strip(), "Confidence Score:", confidence_score)

        # Display the frame
        cv2.imshow("Webcam", frame)

        # write to arduino
        class_number, class_string = class_name.split(" ")
        arduino.write(bytes(class_string.strip(), 'utf-8'))
        # Wait for 1 second (1000 ms) before capturing the next frame
        if cv2.waitKey(1000) & 0xFF == ord("q"):  # Press 'q' to quit
            break

finally:
    # Release the webcam and close the OpenCV window
    cap.release()
    cv2.destroyAllWindows()
