import socket
import pygame

CELL_SIZE = 20
HOST = "127.0.0.1"
PORT = 12345

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

def draw_grid(screen, rows, cols):
    for i in range(rows):
        for j in range(cols):
            rect = pygame.Rect(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE)
            pygame.draw.rect(screen, (50, 50, 50), rect, 1)

def draw_cells(screen, board):
    for i, row in enumerate(board):
        for j, val in enumerate(row):
            rect = pygame.Rect(j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE)
            color = (0, 200, 0) if val == 1 else (30, 30, 30)
            pygame.draw.rect(screen, color, rect)

def main():
    print("[CLIENT] Connecting to server...")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
        sock.connect((HOST, PORT))
        print("[CLIENT] Connected!")
    except Exception as e:
        print(f"[CLIENT] Connection failed: {e}")
        return
    sock.setblocking(False)

    pygame.init()
    screen = None
    rows = cols = 0

    clock = pygame.time.Clock()
    buffer = ""
    running = True

    while running:
        try:
            data = sock.recv(4096).decode()
            buffer += data
            if '\n' in buffer:
                lines = buffer.split('\n')
                board_str = '\n'.join(lines[:-1])
                buffer = lines[-1]

                new_rows, new_cols, board = parse_board(board_str)
                if board and (new_rows != rows or new_cols != cols):
                    rows, cols = new_rows, new_cols
                    screen = pygame.display.set_mode((cols * CELL_SIZE, rows * CELL_SIZE))
                    pygame.display.set_caption("Game of Life")
                    draw_grid(screen, rows, cols)

                if screen:
                    draw_cells(screen, board)
                    pygame.display.flip()

        except BlockingIOError:
            pass

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        clock.tick(60)

    sock.close()
    pygame.quit()

if __name__ == "__main__":
    main()
