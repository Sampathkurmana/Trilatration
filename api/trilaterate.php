<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

require_once '../config.php';

function trilaterate($x1, $y1, $r1, $x2, $y2, $r2, $x3, $y3, $r3) {
    // Calculate the intersection point of three circles
    $ex = array(
        'x' => ($x2 - $x1) / hypot($x2 - $x1, $y2 - $y1),
        'y' => ($y2 - $y1) / hypot($x2 - $x1, $y2 - $y1)
    );
    
    $i = $ex['x'] * ($x3 - $x1) + $ex['y'] * ($y3 - $y1);
    
    $ey = array(
        'x' => ($x3 - $x1 - $i * $ex['x']) / hypot($x3 - $x1 - $i * $ex['x'], $y3 - $y1 - $i * $ex['y']),
        'y' => ($y3 - $y1 - $i * $ey['y']) / hypot($x3 - $x1 - $i * $ex['x'], $y3 - $y1 - $i * $ex['y'])
    );
    
    $d = hypot($x2 - $x1, $y2 - $y1);
    $j = $ey['x'] * ($x3 - $x1) + $ey['y'] * ($y3 - $y1);
    
    $x = (pow($r1, 2) - pow($r2, 2) + pow($d, 2)) / (2 * $d);
    $y = (pow($r1, 2) - pow($r3, 2) + pow($i, 2) + pow($j, 2) - 2 * $j * $y) / (2 * $j) - ($i / $j) * $x;
    
    $result = array(
        'x' => $x1 + $x * $ex['x'] + $y * $ey['x'],
        'y' => $y1 + $x * $ex['y'] + $y * $ey['y']
    );
    
    // Clamp to room boundaries
    $result['x'] = max(0, min(6, $result['x']));
    $result['y'] = max(0, min(5, $result['y']));
    
    return $result;
}

// Get distances from request or fetch from Firebase
$method = $_SERVER['REQUEST_METHOD'];

if ($method === 'POST') {
    $input = json_decode(file_get_contents('php://input'), true);
    
    $distances = array(
        'A' => $input['distance_a'] ?? null,
        'B' => $input['distance_b'] ?? null,
        'C' => $input['distance_c'] ?? null
    );
    
    if ($distances['A'] && $distances['B'] && $distances['C']) {
        $position = trilaterate(
            0, 0, $distances['A'],
            4, 0, $distances['B'],
            2, 3, $distances['C']
        );
        
        echo json_encode([
            'success' => true,
            'position' => $position,
            'timestamp' => time()
        ]);
    } else {
        echo json_encode([
            'success' => false,
            'error' => 'Missing distance values'
        ]);
    }
} else {
    echo json_encode([
        'success' => false,
        'error' => 'Only POST method is supported'
    ]);
}
?> 
