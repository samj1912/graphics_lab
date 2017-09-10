#!/usr/bin/python
import argparse
import turtle

from collections import namedtuple

Center = namedtuple('Center', ['x', 'y'])


def draw_sym(pen, x, y, color="black", center=None):
    pen.setpos(x + center.x, y + center.y)
    pen.dot(1, color)
    pen.setpos(-x + center.x, -y + center.y)
    pen.dot(1, color)


def draw_curve(pen, length, denominator, center=None):
    x = 0.0
    y = 0.0

    draw_sym(pen, x, y, center)

    critical_point = (denominator / 3) ** 0.5

    while x < critical_point and x < length:
        x += 1
        if y + 0.5 - ((x**3) / denominator) < 0:
            y += 1
        draw_sym(pen, x, y, center)

    while x < length:
        y += 1
        if y - ((x + 0.5)**3) / denominator > 0:
            x += 1
        draw_sym(pen, x, y, center)


def draw_curve_incremental(pen, length, denominator, center=None):
    x = 0.0
    y = 0.0

    draw_sym(pen, x, y, "red", center)

    critical_point = (denominator / 3) ** 0.5

    decision_paremeter = y + 0.5 - (x + 1)**3 / denominator

    while x < critical_point and x < length:
        x += 1
        if decision_paremeter < 0:
            decision_paremeter += 1
            y += 1
        decision_paremeter -= (3 * (x**2 + x) + 1) / denominator
        draw_sym(pen, x, y, "red", center)

    x += 0.5
    decision_paremeter = y + 1 - (x**3 / denominator)

    while x < length:
        y += 1
        if decision_paremeter > 0:
            decision_paremeter -= (3 * (x**2 + x) + 1) / denominator
            x += 1
        decision_paremeter += 1
        draw_sym(pen, x - 0.5, y, "red", center)


def main():
    parser = argparse.ArgumentParser(description='Rasterize and display a cubic equation.')
    parser.add_argument('length', type=int,
                        help='Length of the x-axis, till which the equation is to be rasterized.')
    parser.add_argument('denominator', type=int,
                        help='Denominator for the cubic eqation of type "y = x**3 / denominator "')
    parser.add_argument('--mode', type=int, default=1,
                        help="""Mode with which to rasterize the given curve.
                                Values: 1 - Incremental 2 - Basic 3 - Comparison""")
    parser.add_argument('--x', type=int, default=0,
                        help="""X co-ordinate of center""")
    parser.add_argument('--y', type=int, default=0,
                        help="""Y co-ordinate of center""")
    args = parser.parse_args()
    turtle.setup(width=1366, height=768)
    turtle.tracer(0, 0)
    pen = turtle.Turtle()
    pen.up()
    pen.hideturtle()
    pen.speed(0)
    center = Center(args.x, args.y)
    if args.mode % 2:
        draw_curve_incremental(pen, args.length, args.denominator, center)
    if args.mode > 1:
        draw_curve(pen, args.length, args.denominator, center)
    turtle.update()
    turtle.done()


if __name__ == '__main__':
    main()
