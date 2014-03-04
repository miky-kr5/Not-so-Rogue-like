#!/usr/bin/env python

# Copyright (c) 2014, Miguel Angel Astor Romero. All rights reserved.
# See the file LICENSE for more details.

import pygame

SCREEN_SIZE = (800, 600)
TITLE = "Cyberpunk Jam 2014 - Level editor"
FPS = 60

def main():
    # Local variables
    done = False

    # Initialize Pygame and pgs4a.
    pygame.init()
    clock = pygame.time.Clock()
    screen  = pygame.display.set_mode(SCREEN_SIZE, pygame.HWSURFACE | pygame.DOUBLEBUF)
    pygame.mouse.set_visible(False)

    # Main game loop.
    while(not done):
        try:
            fps = clock.get_fps() + 0.001
            pygame.display.set_caption(TITLE + ": " + str(int(fps)))

            # Input capture.
            for event in pygame.event.get():
                if (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE) or event.type == pygame.QUIT:
                        done = True

            # Update cycle.

            # Render cycle.
            screen.fill((0, 0, 0))

            pygame.display.update()
            clock.tick(FPS)

        except KeyboardInterrupt:
            done = True

    pygame.quit()

if __name__ == "__main__":
    main()
