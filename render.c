#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define IMAGE_HEIGHT 1080
#define IMAGE_WIDTH 1920
#define SEGMENTS 150   // теперь это константа, ошибки не будет

struct point_f
{
    float x_;
    float y_;
};

struct color
{
    float r_, g_, b_;
};

struct cube_bezier_curve
{
    struct color curve_color_;
    struct point_f point_[4];
};

struct point_f get_bezier_point(float t, const struct cube_bezier_curve* curve);
struct color get_color(const struct point_f* point, const struct point_f bezier_curve_points[],
    const struct color* bezier_curve_color, int segments);

int main()
{
    // Массив теперь фиксированного размера (константа)
    struct point_f bezier_curve_point_segments[SEGMENTS];

    // МОЯ УНИКАЛЬНАЯ КРИВАЯ И ЦВЕТ
    struct cube_bezier_curve curve = {
        {0.2f, 0.3f, 0.9f},  // Цвет кривой: сине-фиолетовый
        {
            {0.0f, 0.8f},    // P0 - стартовая точка (слева, высоко)
            {0.3f, 0.2f},    // P1 - контрольная точка 1 (нырок вниз)
            {0.7f, 0.9f},    // P2 - контрольная точка 2 (резкий подъём)
            {1.0f, 0.4f}     // P3 - конечная точка (справа, средне)
        }
    };

    /* Предварительно вычисляем точки кривой */
    for (int i = 0; i < SEGMENTS; ++i)
    {
        float t = (float)i / (SEGMENTS - 1);
        bezier_curve_point_segments[i] = get_bezier_point(t, &curve);
    }

    /* Вывод заголовка PPM файла */
    printf("P3\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);

    /* Рендеринг изображения */
    for (int j = 0; j < IMAGE_HEIGHT; ++j)
    {
        for (int i = 0; i < IMAGE_WIDTH; ++i)
        {
            struct point_f point = { (float)i / IMAGE_WIDTH, (float)j / IMAGE_HEIGHT };
            struct color pixel_color = get_color(&point, bezier_curve_point_segments, &curve.curve_color_, SEGMENTS);

            int ri = (int)(pixel_color.r_ * 255.999f);
            int gi = (int)(pixel_color.g_ * 255.999f);
            int bi = (int)(pixel_color.b_ * 255.999f);

            printf("%d %d %d\n", ri, gi, bi);
        }
    }
    return 0;
}

struct point_f get_bezier_point(float t, const struct cube_bezier_curve* curve)
{
    float a = 1 - t;
    float a2 = a * a;
    float a3 = a2 * a;
    float t2 = t * t;
    float t3 = t2 * t;

    struct point_f point;
    point.x_ = a3 * curve->point_[0].x_ +
        3 * a2 * t * curve->point_[1].x_ +
        3 * a * t2 * curve->point_[2].x_ +
        t3 * curve->point_[3].x_;

    point.y_ = a3 * curve->point_[0].y_ +
        3 * a2 * t * curve->point_[1].y_ +
        3 * a * t2 * curve->point_[2].y_ +
        t3 * curve->point_[3].y_;
    return point;
}

float distance(const struct point_f* point_u, const struct point_f* point_v)
{
    float dx = point_u->x_ - point_v->x_;
    float dy = point_u->y_ - point_v->y_;
    return sqrt(dx * dx + dy * dy);
}

struct color get_color(const struct point_f* point, const struct point_f bezier_curve_points[],
    const struct color* bezier_curve_color, int segments)
{
    /* Поиск минимального расстояния от точки до кривой */
    float min_dist = 1.0f;
    for (int i = 0; i < segments; ++i)
    {
        float dist_bet_points = distance(point, &bezier_curve_points[i]);
        if (min_dist > dist_bet_points)
        {
            min_dist = dist_bet_points;
        }
    }

    /* МОЙ УНИКАЛЬНЫЙ ФОН: тёплый персиковый */
    struct color background = { 0.98f, 0.82f, 0.68f };

    /* Sigma для плавности (изменено для другого эффекта) */
    float sigma = 0.18f;

    /* Нормальное распределение для плавного затухания */
    float weight = (float)exp(-min_dist * min_dist / (2 * sigma * sigma));

    /* Смешивание фона и цвета кривой */
    struct color pixel_color = {
        background.r_ * (1 - weight) + bezier_curve_color->r_ * weight,
        background.g_ * (1 - weight) + bezier_curve_color->g_ * weight,
        background.b_ * (1 - weight) + bezier_curve_color->b_ * weight
    };
    return pixel_color;
}