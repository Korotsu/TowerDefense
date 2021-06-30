#include "./../include/my_maths.h"
#include <stdlib.h>

Vector2 create_null_vector()
{
	return Vector2{0.0,0.0};
}

Vector2 Unit_vector_computing(Vector2 vector)
{
	float length = vector_length(vector);
	return Vector2{vector.x/length,vector.y/length};
}

bool Is_equal(Vector2 vector1 , Vector2 vector2 )
{
	if ( ( vector1.x == vector2.x ) && ( vector1.y == vector2.y ) )
		return true;

	return false;
}

float vector_length(Vector2 vector)
{
	return sqrt( pow( vector.x, 2 )  + pow ( vector.y, 2) );
}

float square_length(Vector2 vector)
{
	float length = vector_length(vector);
	return pow( length, 2);
}

float distance_between_pts(Point a, Point b)
{
	return sqrt( pow( b.x - a.x, 2 )  + pow (b.y - a.y, 2) );
}

Vector2 vector_addition(Vector2 vector1, Vector2 vector2)
{
	return Vector2 {vector1.x + vector2.x, vector1.y + vector2.y};
}

Vector2 vector_substraction(Vector2 vector1, Vector2 vector2)
{
	return Vector2 {vector1.x - vector2.x, vector1.y - vector2.y};
}

Vector2 vector_scale(Vector2 vector1, float a)
{
	return Vector2 {vector1.x * a, vector1.y * a};
}

Vector2 vector_negate(Vector2 vector1)
{
	return Vector2 {vector1.x * -1, vector1.y * -1};
}

Vector2 rotate_vector(Vector2 vector1, float angle )
{
	float rad = M_PI/180.f;

	return Vector2 {
					vector1.x * cosf(angle * rad) - vector1.y * sinf(angle * rad),
					vector1.x * sinf(angle * rad) + vector1.y * cosf(angle * rad)
					};
}

Vector2 normal_rotate_90_vector(Vector2 vector1)
{
	return rotate_vector(vector1, -90.f);
}

float dot_product(Vector2 vector1, Vector2 vector2)
{
	return ( vector1.x * vector2.x ) + ( vector1.y * vector2.y ) ;
}

int find_angle(Vector2 vector1, Vector2 vector2)
{
	return dot_product(vector1, vector2)/(vector_length(vector1)*(vector_length(vector2)));
}

Matrix::Matrix(int n, int m)
{
    list = new float[n*m];
    carre = m*m;
}

Matrix::~Matrix()
{
    delete(list);
}

Matrix::Matrix(const Matrix& cv)
{
    list = cv.list;
    m = cv.m;
    n = cv.n;
    carre = cv.carre;
}

int Matrix::accessor(int i, int j, int n)
{
    return n*j+i;
}

float* Matrix::identity(int m, int carre)
{
    float* identity_matrix = new float[carre];
    int j = 1;
    for (int i = 0; i < carre; i++)
    {
        if ( i/j ==  0)
        {
            identity_matrix[i] = 1;
        }

        else
        {
            identity_matrix[i] = 0;
        }
        
        if ( i % m == 0)
        {
            j++;
        }
    }
    return identity_matrix;
}

float* Matrix::zero(int n, int m)
{
    float* null_matrix = new float[n*m];
    for (int i = 0; i < n*m; i++)
    {
        null_matrix[i] = 0;
    }
    
    return null_matrix;
}

Matrix Matrix::transposition(Matrix& cv)
{
    float* T_Matrix = (float*) malloc(cv.carre * sizeof(float));
    int i = 0; 
    int k = 0;
    while ( i < cv.carre )
    {
        for (int j = 0; j < cv.n ; j++)
        {
            T_Matrix[i] = cv.list[k];
            k += cv.m;
            i++;
        }
        k -= cv.n * cv.m;
        k++;
    }
    return cv;
}