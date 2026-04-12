#pragma once

#include "Vectors.h"

namespace CoreMath
{
	typedef struct Mat2
	{
		union
		{
			struct
			{
				float _11, _12,
					_21, _22;
			};
			float asArray[4];
		};

		inline Mat2()
		{
			_11 = _22 = 1.0f;
			_12 = _21 = 0.0f;
		}

		inline Mat2(float f11, float f12,
			float f21, float f22)
		{
			_11 = f11; _12 = f12;
			_21 = f21; _22 = f22;
		}

		inline float* operator[] (int i)
		{
			return &(asArray[i * 2]);
		}

		inline const float* operator[](int col) const
		{
			return &asArray[col * 2];
		}
	} Mat2;

	typedef struct Mat3
	{
		union
		{
			struct
			{
				float _11, _12, _13,
					_21, _22, _23,
					_31, _32, _33;
			};
			float asArray[9];
		};

		inline Mat3()
		{
			_11 = _22 = _33 = 1.0f;
			_12 = _13 = _21 = 0.0f;
			_23 = _31 = _32 = 0.0f;
		}

		inline Mat3(float f11, float f12, float f13,
			float f21, float f22, float f23,
			float f31, float f32, float f33)
		{
			_11 = f11; _12 = f12; _13 = f13;
			_21 = f21; _22 = f22; _23 = f23;
			_31 = f31; _32 = f32; _33 = f33;
		}

		inline float* operator[] (int i)
		{
			return &(asArray[i * 3]);
		}

		inline const float* operator[](int col) const
		{
			return &asArray[col * 3];
		}
	} Mat3;

	typedef struct Mat4
	{
		union
		{
			struct
			{
				float _11, _12, _13, _14,
					_21, _22, _23, _24,
					_31, _32, _33, _34,
					_41, _42, _43, _44;
			};
			float asArray[16];
		};

		inline Mat4()
		{
			_11 = _22 = _33 = _44 = 1.0f;
			_12 = _13 = _14 = _21 = 0.0f;
			_23 = _24 = _31 = _32 = 0.0f;
			_34 = _41 = _42 = _43 = 0.0f;
		}

		inline Mat4(float f11, float f12, float f13, float f14,
			float f21, float f22, float f23, float f24,
			float f31, float f32, float f33, float f34,
			float f41, float f42, float f43, float f44)
		{
			_11 = f11; _12 = f12; _13 = f13; _14 = f14;
			_21 = f21; _22 = f22; _23 = f23; _24 = f24;
			_31 = f31; _32 = f32; _33 = f33; _34 = f34;
			_41 = f41; _42 = f42; _43 = f43; _44 = f44;
		}

		inline float* operator[] (int i)
		{
			return &(asArray[i * 4]);
		}

		inline const float* operator[](int col) const
		{
			return &asArray[col * 4];
		}
	} Mat4;

	void Transpose(const float* srcMat, float* dstMat,
		int srcRows, int srcCols);
	Mat2 Transpose(const Mat2& matrix);
	Mat3 Transpose(const Mat3& matrix);
	Mat4 Transpose(const Mat4& matrix);

	Mat2 operator* (const Mat2& matrix, float scalar);
	Mat3 operator* (const Mat3& matrix, float scalar);
	Mat4 operator* (const Mat4& matrix, float scalar);

	Mat2 operator* (const Mat2& matA, const Mat2& matB);
	Mat3 operator* (const Mat3& matA, const Mat3& matB);
	Mat4 operator* (const Mat4& matA, const Mat4& matB);

	float Determinant(const Mat2& matrix);
	float Determinant(const Mat3& matrix);

	Mat2 Cut(const Mat3& mat, int row, int col);
	Mat2 Minor(const Mat2& mat);
	Mat3 Minor(const Mat3& mat);

	void Cofactor(float* out, const float* minor,
		int rowa, int cols);
	Mat3 Cofactor(const Mat3& mat);
	Mat2 Cofactor(const Mat2& mat);

	Mat3 Cut(const Mat4& mat, int row, int col);
	Mat4 Minor(const Mat4& mat);
	Mat4 Cofactor(const Mat4& mat);
	float Determinant(const Mat4& mat);

	Mat2 Adjugate(const Mat2& mat);
	Mat3 Adjugate(const Mat3& mat);
	Mat4 Adjugate(const Mat4& mat);

	Mat2 Inverse(const Mat2& mat);
	Mat3 Inverse(const Mat3& mat);
	Mat4 Inverse(const Mat4& mat);

	Mat4 Translation(float x, float y, float z);
	Mat4 Translation(const Vec3& pos);
	Vec3 GetTranslation(const Mat4& mat);

	Mat4 Scale(float x, float y, float z);
	Mat4 Scale(const Vec3& vec);
	Vec3 GetScale(const Mat4& mat);

	Mat4 Rotation(float pitch, float yaw, float roll);
	Mat3 Rotation3x3(float pitch, float yaw, float roll);

	Mat4 ZRotation(float angle);
	Mat3 ZRotation3x3(float angle);
	Mat4 XRotation(float angle);
	Mat3 XRotation3x3(float angle);
	Mat4 YRotation(float angle);
	Mat3 YRotation3x3(float angle);

	Mat4 AxisAngle(const Vec3& axis, float angle);
	Mat3 AxisAngle3x3(const Vec3& axis, float angle);

	Vec3 MultiplyMat4Point(const Mat4& mat, const Vec3& vec);
	Vec3 MultiplyMat4Vec3(const Mat4& mat, const Vec3& vec);
	Vec3 MultiplyMat3Vec3(const Mat3& mat, const Vec3& vec);

	Mat4 Transform(const Vec3& scale, const Vec3& eulerRotation, 
		const Vec3& translate);
	Mat4 Transform(const Vec3& scale, const Vec3& rotationAxis, 
		float rotationAngle, const Vec3& translate);

	Mat4 LookAt(const Vec3& position, const Vec3& target, const Vec3& up);

	Mat4 Projection(float fov, float aspect,
		float zNear, float zFar);
	Mat4 Ortho(float left, float right, float bottom,
		float top, float zNear, float zFar);
}