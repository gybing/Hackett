#pragma once

/*
 �a�����η�ؐƝ�����ĳ�ʽ�a
*/
 
typedef struct
{
    float x;
    float y;
}
Point2D;
 
/*
 cp�ڴ����Ă�Ԫ�ص����:
 cp[0]����ʼ�c�����ψD�е�P0
 cp[1]���һ�������c�����ψD�е�P1
 cp[2]��ڶ��������c�����ψD�е�P2
 cp[3]��Y���c�����ψD�е�P3
 t�酢��ֵ��0 <= t <= 1
*/
 
Point2D PointOnCubicBezier( Point2D* cp, float t )
{
    float ax, bx, cx;
    float ay, by, cy;
    float tSquared, tCubed;
    Point2D result;
 
    /*Ӌ����ʽ�S��*/
 
    cx = 3.0 * (cp[1].x - cp[0].x);
    bx = 3.0 * (cp[2].x - cp[1].x) - cx;
    ax = cp[3].x - cp[0].x - cx - bx;
 
    cy = 3.0 * (cp[1].y - cp[0].y);
    by = 3.0 * (cp[2].y - cp[1].y) - cy;
    ay = cp[3].y - cp[0].y - cy - by;
 
    /*Ӌ��λ춅���ֵt�������c*/
 
    tSquared = t * t;
    tCubed = tSquared * t;
 
    result.x = (ax * tCubed) + (bx * tSquared) + (cx * t) + cp[0].x;
    result.y = (ay * tCubed) + (by * tSquared) + (cy * t) + cp[0].y;
 
    return result;
}
 
/*
 ComputeBezier�Կ����ccp���a���������c������Point2D�Y������С�
 �����߱�횷�������ӛ���w�Թ�ݔ���Y�������<sizeof(Point2D) numberOfPoints>
*/
 
void ComputeBezier( Point2D* cp, int numberOfPoints, Point2D* curve )
{
    float dt;
    int i;
 
    dt = 1.0 / ( numberOfPoints - 1 );
 
    for( i = 0; i < numberOfPoints; i++)
        curve[i] = PointOnCubicBezier( cp, i*dt );
}