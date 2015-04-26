from pygame.locals import *
import pygame, os, sys
import serial
import time
import matplotlib.pyplot as plt
pygame.init()
ser = serial.Serial("/dev/ttyACM0", 9600)

WIDTH = 640
HEIGHT = 480
data = []


class sliderBack():
    def __init__(self,name,x, y, width, height):
        self.name = name
        self.view = pygame.Surface((width, height))
        self.rect = self.view.get_rect()
        
        self.view.fill((0,255,0))

        self.rect.topleft = (x, y)

class sliderBar():
    def __init__(self,name,x, y, width, height):
        self.name = name
        self.view = pygame.Surface((width, height))
        self.rect = self.view.get_rect()
        
        self.view.fill((255,0,0))
        self.rect.midleft = (x,y)

class Button():
    def __init__(self, name, x, y, width, height):
        self.name = name
        self.view = pygame.Surface((width, height))
        self.rect = self.view.get_rect()
        
        self.view.fill((0,0,255))

        self.rect.topleft = (x, y)

P = 0
I = 0
D = 0

scr = pygame.display.set_mode((WIDTH, HEIGHT))

pygame.display.set_caption('Box Test')

backs = []
bars = []
labels = []


back1 = sliderBack("ProportionalBack", 30,HEIGHT/4, WIDTH-200, 50)
back2 = sliderBack("IntegralBack", 30, 2*HEIGHT/4, WIDTH-200, 50)
back3 = sliderBack("IntegralBack", 30, 3*HEIGHT/4, WIDTH-200, 50)

bar1 = sliderBar("ProportionalBar", 30, back1.rect.centery, 25, 60)
bar2 = sliderBar("IntegralBar", 30, back2.rect.centery, 25, 60)
bar3 = sliderBar("DerivativeBar", 30, back3.rect.centery, 25, 60)

button = Button("Display", back2.rect.topright[0] + 100, back2.rect.topright[1], 64, 64)

myfont = pygame.font.Font(None, 50)
bigfont = pygame.font.Font(None, 50)


label_title = bigfont.render("PID Control", 1, (255, 0, 0))
label_go = myfont.render("GO", 1, (250, 250, 240))
label_proportional = myfont.render(str(P), 1, (255, 0, 0))
label_integral = myfont.render(str(I), 1, (255, 0, 0))
label_derivative = myfont.render(str(D), 1, (255, 0, 0))
label_P = myfont.render("P", 1, (255, 250, 240))
label_I = myfont.render("I", 1, (255, 250, 240))
label_D = myfont.render("D", 1, (255, 250, 240))

backs.append(back1)
backs.append(back2)
backs.append(back3)

bars.append(bar1)
bars.append(bar2)
bars.append(bar3)

clicked = False


plt.plot(data)


while True:
    #read = ser.readline()
    
    #try:
    #     data.append(int(read))
    # except:
    #     print 'meh'

    # print read
    # plt.plot(data)
    # plt.draw()
    # plt.ion()
    # plt.show()
    

    pygame.display.update()
    scr.fill((0, 0, 0))
    
    for b in backs:
        scr.blit(b.view, b.rect)

    for b in bars:
        scr.blit(b.view, b.rect)

    scr.blit(button.view, button.rect)
    
    scr.blit(label_proportional, (back1.rect.midright[0] + 10,back1.rect.midright[1]-20))
    scr.blit(label_integral, (back2.rect.midright[0] + 10,back2.rect.midright[1]-20))
    scr.blit(label_derivative, (back3.rect.midright[0] + 10,back3.rect.midright[1]-20))
    scr.blit(label_go, (button.rect.topleft[0] + 5 , button.rect.topleft[1] + 10))

    scr.blit(label_P, (0, back1.rect.topleft[1]+ 10))
    scr.blit(label_I, (0, back2.rect.topleft[1]+ 10))
    scr.blit(label_D, (0, back3.rect.topleft[1]+ 10))

    scr.blit(label_title, (WIDTH/2 - 100,0))
  

    if pygame.mouse.get_pressed()[0]!=0:

        if button.rect.collidepoint(pygame.mouse.get_pos()) and clicked == False:
            clicked = True
            print "P = " + str(P) + "\nI = " + str(I) + "\nD = " + str(D)
            ser.write("P"+str(P)+"I"+str(I)+"D"+str(D))
            print "sent"


        if not (button.rect.collidepoint(pygame.mouse.get_pos())) and clicked == True:
            clicked = False            

        else:
            for i in range (len(bars)):
                bar = bars[i]
                rect = bar.rect
                rect_back = backs[i].rect
                
                if rect.collidepoint(pygame.mouse.get_pos()):
                    rect.centerx = pygame.mouse.get_pos()[0]
                    if rect.midright[0] > rect_back.midright[0]:
                        rect.midright = (rect_back.midright)
                    if rect.midleft[0] < rect_back.midleft[0]:
                        rect.midleft = (rect_back.midleft)

                pos = rect.centerx


                if bar.name == "ProportionalBar":
                    rect = bar.rect
                    if not (rect.x == P):
                        P = rect.x - 30
                        label_proportional = myfont.render(str(P), 1, (255,0,0))
                        #print "P = " + str(P)
                if bar.name == "IntegralBar" and rect.x != I:
                    rect = bar.rect
                    if not (rect.x == I):
                        I = rect.x - 30
                        label_integral = myfont.render(str(I), 1, (255,0,0))
                        #print "I = " + str(I)
                if bar.name == "DerivativeBar" and rect.x != D:
                    rect = bar.rect
                    if not (rect.x == D):
                        D = rect.x - 30
                        label_derivative = myfont.render(str(D), 1, (255,0,0))



        
 


    for event in pygame.event.get():
        if event.type == QUIT:
            pygame.quit()
            sys.exit()