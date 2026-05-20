<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, OPTIONS');
header('Access-Control-Allow-Headers: Content-Type');

require_once '../config.php';

function trilaterate($x1, $y1, $r1, $x2, $y2, $r2, $x3, $y3, $r3) {
    // Check for valid inputs
    if (!$r1 || !$r2 || !$r3 || $r1 <= 0 || $r2 <= 0 || $r3 <= 0) {
        return null;
    }
    
    try {
        // Calculate intersection of three circles
        $A = 2 * ($x2 - $x1);
        $B = 2 * ($y2 - $y1);
        $C = pow($r1, 2) - pow($r2, 2) - pow($x1, 2) + pow($x2, 2) - pow($y1, 2) + pow($y2, 2);
        
        $D = 2 * ($x3 - $x2);
        $E = 2 * ($y3 - $y2);
        $F = pow($r2, 2) - pow($r3, 2) - pow($x2, 2) + pow($x3, 2) - pow($y2, 2) + pow($y3, 2);
        
        $det = $E * $A - $B * $D;
        
        if (abs($det) < 0.0001) {
            return null; // Parallel lines, no intersection
        }
        
        $x = ($C * $E - $F * $B) / $det;
        $y = ($C * $D - $A * $F) / ($B * $D - $A * $E);
        
        // Clamp to room boundaries (0-6 for x, 0-5 for y)
        $x = max(0, min(6, $x));
        $y = max(0, min(5, $y));
        
        return [
            'x' => round($x, 2),
            'y' => round($y, 2)
        ];
    } catch (Exception $e) {
        return null;
    }
}

// Handle request
$method = $_SERVER['REQUEST_METHOD'];

if ($method === 'GET') {
    // Get distances from query parameters
    $d1 = isset($_GET['d1']) ? floatval($_GET['d1']) : null;
    $d2 = isset($_GET['d2']) ? floatval($_GET['d2']) : null;
    $d3 = isset($_GET['d3']) ? floatval($_GET['d3']) : null;
    
    if ($d1 && $d2 && $d3) {
        $position = trilaterate(0, 0, $d1, 4, 0, $d2, 2, 3, $d3);
        
        echo json_encode([
            'success' => true,
            'position' => $position,
            'distances' => ['A' => $d1, 'B' => $d2, 'C' => $d3],
            'timestamp' => time()
        ]);
    } else {
        echo json_encode([
            'success' => false,
            'error' => 'Missing distance parameters. Use: d1, d2, d3'
        ]);
    }
    
} elseif ($method === 'POST') {
    $input = json_decode(file_get_contents('php://input'), true);
    
    if ($input) {
        $d1 = $input['distance_a'] ?? $input['d1'] ?? null;
        $d2 = $input['distance_b'] ?? $input['d2'] ?? null;
        $d3 = $input['distance_c'] ?? $input['d3'] ?? null;
        
        if ($d1 && $d2 && $d3) {
            $position = trilaterate(0, 0, $d1, 4, 0, $d2, 2, 3, $d3);
            
            echo json_encode([
                'success' => true,
                'position' => $position,
                'distances' => ['A' => $d1, 'B' => $d2, 'C' => $d3],
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
            'error' => 'Invalid JSON input'
        ]);
    }
    
} else {
    echo json_encode([
        'success' => false,
        'error' => 'Only GET and POST methods are supported'
    ]);
}
?>