#!/usr/bin/python
import argparse
import turtle


def draw_sym(pen, x, y, color="black"):
    pen.setpos(x, y)
    pen.dot(1, color)
    pen.setpos(-x, -y)
    pen.dot(1, color)


def draw_curve(pen, length, denominator):
    x = 0.0
    y = 0.0
    draw_sym(pen, x, y)
    critical_point = int((denominator / 3) ** 0.5)
    while x < critical_point and x <= length:
        x += 1
        if y + 0.5 - ((x**3) / denominator) < 0:
            y += 1
        draw_sym(pen, x, y)
    while x <= length:
        y += 1
        if y - ((x + 0.5)**3) / denominator > 0:
            x += 1
        draw_sym(pen, x, y)


def draw_curve_incremental(pen, length, denominator):
    x = 0.0
    y = 0.0
    draw_sym(pen, x, y)

    critical_point = int((denominator / 3) ** 0.5)

    prev_x_cubed = x**3
    next_x_cubed = (x + 1)**3
    decision_paremeter = y + 0.5 - next_x_cubed / denominator

    while x < critical_point and x <= length:
        prev_x_cubed = next_x_cubed
        next_x_cubed = prev_x_cubed + 3 * (x**2 + x) + 1
        x += 1
        if decision_paremeter < 0:
            y += 1
            decision_paremeter += 1
        decision_paremeter += (prev_x_cubed - next_x_cubed) / denominator
        draw_sym(pen, x, y, "red")

    next_x_cubed = (x + 0.5)**3
    decision_paremeter = y + 1 - next_x_cubed / denominator

    while x <= length:
        y += 1
        if decision_paremeter > 0:
            x += 1
            prev_x_cubed = next_x_cubed
            next_x_cubed = prev_x_cubed + 3 * (x**2 + x) + 1
            decision_paremeter += (prev_x_cubed - next_x_cubed) / denominator
        decision_paremeter += 1
        draw_sym(pen, x, y, "red")


def main():
    parser = argparse.ArgumentParser(description='Rasterize and display a cubic equation.')
    parser.add_argument('length', type=int,
                        help='Length of the x-axis, till which the equation is to be rasterized.')
    parser.add_argument('denominator', type=int,
                        help='Denominator for the cubic eqation of type "y = x**3 / denominator "')
    parser.add_argument('--mode', type=int, default=1,
                        help="""Mode with which to rasterize the given curve.
                                Values: 1 - Incremental 2 - Basic 3 - Comparison""")
    args = parser.parse_args()
    turtle.setup(width=1366, height=768)
    turtle.tracer(0, 0)
    pen = turtle.Turtle()
    pen.up()
    pen.hideturtle()
    pen.speed(0)
    if args.mode % 2:
        draw_curve_incremental(pen, args.length, args.denominator)
    if args.mode > 1:
        draw_curve(pen, args.length, args.denominator)
    turtle.update()
    turtle.done()


if __name__ == '__main__':
    main()
