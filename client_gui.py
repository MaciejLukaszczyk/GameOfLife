import socket
import pygame
import math

HOST = "127.0.0.1"
PORT = 12345
CELL_RADIUS = 15  # promień heksagonu
CELL_HEIGHT = math.sqrt(3) * CELL_RADIUS
CELL_WIDTH = 2 * CELL_RADIUS
HORIZ_SPACING = CELL_WIDTH * 3/4
VERT_SPACING = CELL_HEIGHT

def hex_corner(x, y, size, i):
    angle_deg = 60 * i - 30
    angle_rad = math.pi / 180 * angle_deg
    return (x + size * math.cos(angle_rad), y + size * math.sin(angle_rad))

def draw_hexagon(surface, x, y, size, color):
    points = [hex_corner(x, y, size, i) for i in range(6)]
    pygame.draw.polygon(surface, color, points)
    pygame.draw.polygon(surface, (50, 50, 50), points, 1)  # outline

def parse_board(data):
    lines = [line.strip() for line in data.strip().splitlines() if line.strip()]
    if not lines:
        return None, None, []

    dims = lines[0].split()
    rows, cols = int(dims[0]), int(dims[1])
    board = [list(map(int, line.split())) for line in lines[1:]]
    return rows, cols, board

def main():
    print("[SERVER] Waiting for connection...")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((HOST, PORT))
    sock.listen(1)

    conn, addr = sock.accept()
    print(f"[SERVER] Connected to {addr}")
    conn.setblocking(False)

    pygame.init()
    screen = None
    rows = cols = 0

    clock = pygame.time.Clock()
    buffer = ""
    running = True

    while running:
        try:
            data = conn.recv(4096).decode()
            buffer += data
            if "\n\n" in buffer:
                chunks = buffer.split("\n\n")
                for chunk in chunks[:-1]:
                    new_rows, new_cols, board = parse_board(chunk)
                    if board:
                        if new_rows != rows or new_cols != cols:
                            rows, cols = new_rows, new_cols
                            width = int(cols * HORIZ_SPACING + CELL_RADIUS)
                            height = int(rows * VERT_SPACING + CELL_RADIUS)
                            screen = pygame.display.set_mode((width, height))
                            pygame.display.set_caption("Hexagonal Game of Life")
                        if screen:
                            screen.fill((0, 0, 0))
                            for i, row in enumerate(board):
                                for j, val in enumerate(row):
                                    x = j * HORIZ_SPACING + CELL_RADIUS
                                    y = i * VERT_SPACING + CELL_RADIUS
                                    if i % 2 == 1:
                                        x += HORIZ_SPACING / 2  # offset for odd rows
                                    color = (0, 200, 0) if val == 1 else (30, 30, 30)
                                    draw_hexagon(screen, x, y, CELL_RADIUS, color)
                            pygame.display.flip()
                buffer = chunks[-1]
        except BlockingIOError:
            pass

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        clock.tick(60)

    conn.close()
    sock.close()
    pygame.quit()

if __name__ == "__main__":
    main()
