<?php
// Firebase Configuration
// Note: For security, it's better to use environment variables or a .env file

define('FIREBASE_API_KEY', 'AIzaSyC1w-4gd_En42m4DObFs1JM8rCSYsj9BXM');
define('FIREBASE_AUTH_DOMAIN', 'triangulation-4be54.firebaseapp.com');
define('FIREBASE_DATABASE_URL', 'https://triangulation-4be54-default-rtdb.firebaseio.com');
define('FIREBASE_PROJECT_ID', 'triangulation-4be54');
define('FIREBASE_STORAGE_BUCKET', 'triangulation-4be54.firebasestorage.app');
define('FIREBASE_MESSAGING_SENDER_ID', '743532804404');
define('FIREBASE_APP_ID', '1:743532804404:web:b20fbee10f1ff849e04d61');
define('FIREBASE_MEASUREMENT_ID', 'G-9T20KV58W9');

// System Settings
define('SYSTEM_NAME', 'ESP32 Triangulation System');
define('REFRESH_INTERVAL', 2); // seconds
define('MAX_LOG_ENTRIES', 100);

// Tower Positions (meters)
$TOWER_POSITIONS = [
    'A' => ['x' => 0, 'y' => 0],
    'B' => ['x' => 4, 'y' => 0],
    'C' => ['x' => 2, 'y' => 3]
];

// RSSI to Distance calibration parameters
define('RSSI_AT_1M', -42); // Calibrate this value
define('PATH_LOSS_EXPONENT', 2.5); // Environment factor

// Signal quality thresholds
define('SIGNAL_EXCELLENT', -50);
define('SIGNAL_GOOD', -60);
define('SIGNAL_FAIR', -70);
?>