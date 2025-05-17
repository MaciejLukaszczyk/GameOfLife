import socket
import time
import pygame
import math

MAX_WINDOW_WIDTH = 1200
MAX_WINDOW_HEIGHT = 800
LEGEND_WIDTH = 200
CELL_COLORS = {
    0: (30, 30, 30),
    1: (100, 255, 100),
    2: (0, 180, 0),
    3: (0, 100, 255),
    4: (255, 80, 80),
}
HOST = "127.0.0.1"
PORT = 12345
TIMEOUT_SECONDS = 300

window_width = MAX_WINDOW_WIDTH
window_height = MAX_WINDOW_HEIGHT

def parse_board(data):
    lines = data.strip().splitlines()
    if not lines:
        return None, None, []

    dims = lines[0].split()
    rows, cols = int(dims[0]), int(dims[1])
    board = []

    for line in lines[1:]:
        row = [int(x) for x in line.strip().split()]
        board.append(row)

    return rows, cols, board

def hexagon_points(cx, cy, r):
    return [
        (cx + r * math.cos(math.radians(60 * i)),
         cy + r * math.sin(math.radians(60 * i)))
        for i in range(6)
    ]

def draw_hex_cells(surface, board, radius):
    height = math.sqrt(3) * radius
    rows = len(board)
    cols = len(board[0]) if rows > 0 else 0

    grid_width = cols * radius * 1.5 + radius / 2
    grid_height = rows * height + height / 2

    offset_x = (window_width - LEGEND_WIDTH - grid_width) / 2
    offset_y = (window_height - grid_height) / 2

    for row, line in enumerate(board):
        for col, val in enumerate(line):
            x_offset = radius * 3/2 * col
            y_offset = height * row + (height / 2 if col % 2 else 0)
            cx = offset_x + x_offset + radius
            cy = offset_y + y_offset + radius
            points = hexagon_points(cx, cy, radius)
            color = CELL_COLORS.get(val, (255, 0, 255))
            pygame.draw.polygon(surface, color, points)
            pygame.draw.polygon(surface, (50, 50, 50), points, 1)

def draw_legend(surface):
    font = pygame.font.SysFont(None, 20)
    labels = [
        (0, "DEAD (martwa)"),
        (1, "YOUNG (dorastająca)"),
        (2, "REPRODUCTIVE (rozpłodowa)"),
        (3, "OLD_REPRO (stara rozpłodowa)"),
        (4, "DYING (umierająca)"),
    ]
    start_x = window_width - LEGEND_WIDTH + 10
    start_y = 20

    for i, (state, label) in enumerate(labels):
        color = CELL_COLORS.get(state, (255, 0, 255))
        rect = pygame.Rect(start_x, start_y + i * 25, 15, 15)
        pygame.draw.rect(surface, color, rect)
        text = font.render(label, True, (220, 220, 220))
        surface.blit(text, (start_x + 20, start_y + i * 25))

def draw_waiting_text(screen):
    font = pygame.font.SysFont(None, 48)
    text = font.render("I'm waiting for Data :)", True, (200, 200, 200))
    rect = text.get_rect(center=(window_width // 2, window_height // 2))
    screen.fill((0, 0, 0))
    screen.blit(text, rect)
    pygame.display.flip()

def try_connect_with_timeout():
    print("[CLIENT] Trying to connect to server...")
    start_time = time.time()
    while time.time() - start_time < TIMEOUT_SECONDS:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect((HOST, PORT))
            sock.setblocking(False)
            print("[CLIENT] Connected!")
            return sock
        except (socket.timeout, ConnectionRefusedError):
            print("[CLIENT] Server not available, retrying...")
            time.sleep(5)
    print("[CLIENT] Failed to connect after 5 minutes.")
    return None

def main():
    global window_width, window_height
    pygame.init()
    screen = pygame.display.set_mode((window_width, window_height), pygame.DOUBLEBUF)
    pygame.display.set_caption("Hexagonal Game of Life")
    draw_waiting_text(screen)

    sock = try_connect_with_timeout()
    if not sock:
        return

    clock = pygame.time.Clock()
    buffer = ""
    running = True
    waiting = True

    while running:
        try:
            data = sock.recv(32768).decode()
            buffer += data
            while "\n\n" in buffer:
                board_str, buffer = buffer.split("\n\n", 1)
                new_rows, new_cols, board = parse_board(board_str)

                if not (board and 1 <= new_rows <= 500 and 1 <= new_cols <= 500):
                    print(f"[CLIENT] Invalid board size: {new_rows}x{new_cols} — skipping.")
                    continue

                waiting = False
                rows, cols = new_rows, new_cols
                alive = sum(cell > 0 for row in board for cell in row)
                print(f"[CLIENT] Parsed {rows}x{cols}, alive: {alive}")

                available_width = MAX_WINDOW_WIDTH - LEGEND_WIDTH - 40
                available_height = MAX_WINDOW_HEIGHT - 40
                r_w = available_width / (cols * 1.5 + 0.5)
                r_h = available_height / (rows * math.sqrt(3) + 0.5)
                radius = int(min(r_w, r_h))
                if radius < 4 or radius > 40:
                    print(f"[CLIENT] Skipping frame due to unusual radius: {radius}px")
                    continue

                grid_width = cols * radius * 1.5 + radius / 2
                grid_height = rows * math.sqrt(3) * radius + radius / 2
                window_width = min(int(grid_width + LEGEND_WIDTH + 40), MAX_WINDOW_WIDTH)
                window_height = min(int(grid_height + 40), MAX_WINDOW_HEIGHT)

                screen = pygame.display.set_mode((window_width, window_height), pygame.DOUBLEBUF)

                # nowy bufor rysowania
                frame_surface = pygame.Surface((window_width, window_height))
                frame_surface.fill((0, 0, 0))
                draw_hex_cells(frame_surface, board, radius)
                draw_legend(frame_surface)
                screen.blit(frame_surface, (0, 0))
                pygame.display.flip()

        except BlockingIOError:
            pass

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        if waiting:
            draw_waiting_text(screen)

        clock.tick(60)

    sock.close()
    pygame.quit()

if __name__ == "__main__":
    main()
