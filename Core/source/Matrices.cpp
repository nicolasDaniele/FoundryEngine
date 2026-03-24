#include "Matrices.h"
#include "MathDefinitions.h"

namespace CoreMath
{
	void Transpose(const float* srcMat, float* dstMat,
		int srcCols, int srcRows)
	{
		for (int i = 0; i < srcCols * srcRows; i++)
		{
			int col = i % srcCols;
			int row = i / srcCols;
			dstMat[i] = srcMat[srcRows * row + col];
		}
	}

	Mat2 Transpose(const Mat2& matrix)
	{
		Mat2 result;
		Transpose(matrix.asArray, result.asArray, 2, 2);
		return result;
	}

	Mat3 Transpose(const Mat3& matrix)
	{
		Mat3 result;
		Transpose(matrix.asArray, result.asArray, 3, 3);
		return result;
	}

	Mat4 Transpose(const Mat4& matrix)
	{
		Mat4 result;
		Transpose(matrix.asArray, result.asArray, 4, 4);
		return result;
	}

	Mat2 operator* (const Mat2& matrix, float scalar)
	{
		Mat2 result;
		for (int i = 0; i < 4; i++)
		{
			result.asArray[i] = matrix.asArray[i] * scalar;
		}
		return result;
	}

	Mat3 operator* (const Mat3& matrix, float scalar)
	{
		Mat3 result;
		for (int i = 0; i < 9; i++)
		{
			result.asArray[i] = matrix.asArray[i] * scalar;
		}
		return result;
	}

	Mat4 operator* (const Mat4& matrix, float scalar)
	{
		Mat4 result;
		for (int i = 0; i < 16; i++)
		{
			result.asArray[i] = matrix.asArray[i] * scalar;
		}
		return result;
	}

	Mat2 operator* (const Mat2& matA, const Mat2& matB)
	{
		Mat2 result;

		for (int col = 0; col < 2; ++col)
		{
			for (int row = 0; row < 2; ++row)
			{
				result[col][row] =
					matA[0][row] * matB[col][0] +
					matA[1][row] * matB[col][1];
			}
		}

		return result;
	}

	Mat3 operator* (const Mat3& matA, const Mat3& matB)
	{
		Mat3 result;

		for (int col = 0; col < 3; ++col)
		{
			for (int row = 0; row < 3; ++row)
			{
				result[col][row] =
					matA[0][row] * matB[col][0] +
					matA[1][row] * matB[col][1] +
					matA[2][row] * matB[col][2];
			}
		}

		return result;
	}

	Mat4 operator* (const Mat4& matA, const Mat4& matB)
	{
		Mat4 result;

		for (int col = 0; col < 4; ++col)
		{
			for (int row = 0; row < 4; ++row)
			{
				result[col][row] =
					matA[0][row] * matB[col][0] +
					matA[1][row] * matB[col][1] +
					matA[2][row] * matB[col][2] +
					matA[3][row] * matB[col][3];
			}
		}

		return result;
	}

	float Determinant(const Mat2& matrix)
	{
		return matrix._11 * matrix._22 -
			matrix._12 * matrix._21;
	}

	float Determinant(const Mat3& matrix)
	{
		float result = 0.0f;
		Mat3 cofactor = Cofactor(matrix);

		for (int j = 0; j < 3; j++)
		{
			int index = 3 * 0 + j;
			result += matrix.asArray[index] * cofactor[0][j];
		}

		return result;
	}

	Mat2 Cut(const Mat3& mat, int col, int row)
	{
		Mat2 result;
		int index = 0;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (i == row || j == col)
				{
					continue;
				}
				int target = index++;
				int source = 3 * i + j;
				result.asArray[target] = mat.asArray[source];
			}
		}

		return result;
	}

	Mat2 Minor(const Mat2& mat)
	{
		return Mat2(mat._22, mat._21,
			mat._12, mat._11);
	}

	Mat3 Minor(const Mat3& mat)
	{
		Mat3 result;

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				result[i][j] = Determinant(Cut(mat, i, j));
			}
		}

		return result;
	}

	void Cofactor(float* out, const float* minor,
		int cols, int rows)
	{
		for (int i = 0; i < cols; i++)
		{
			for (int j = 0; j < rows; j++)
			{
				int t = rows * j + i;
				int s = rows * j + i;
				float sign = POW(-1.0, i + j);
				out[t] = minor[s] * sign;
			}
		}
	}

	Mat2 Cofactor(const Mat2& mat)
	{
		Mat2 result;
		Cofactor(result.asArray, Minor(mat).asArray, 2, 2);
		return result;
	}

	Mat3 Cofactor(const Mat3& mat)
	{
		Mat3 result;
		Cofactor(result.asArray, Minor(mat).asArray, 3, 3);
		return result;
	}

	Mat3 Cut(const Mat4& mat, int col, int row)
	{
		Mat3 result;
		int index = 0;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (i == col || j == row)
				{
					continue;
				}
				int target = index++;
				int source = 4 * i + j;
				result.asArray[target] = mat.asArray[source];
			}
		}

		return result;
	}

	Mat4 Minor(const Mat4& mat)
	{
		Mat4 result;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result[i][j] = Determinant(Cut(mat, i, j));
			}
		}

		return result;
	}

	Mat4 Cofactor(const Mat4& mat)
	{
		Mat4 result;
		Cofactor(result.asArray, Minor(mat).asArray, 4, 4);
		return result;
	}

	float Determinant(const Mat4& mat)
	{
		float result = 0.0f;
		Mat4 cofactor = Cofactor(mat);

		for (int i = 0; i < 4; i++)
		{
			result += mat.asArray[4 * 0 + i] * cofactor[0][i];
		}

		return result;
	}

	Mat2 Adjugate(const Mat2& mat)
	{
		return Transpose(Cofactor(mat));
	}

	Mat3 Adjugate(const Mat3& mat)
	{
		return Transpose(Cofactor(mat));
	}

	Mat4 Adjugate(const Mat4& mat)
	{
		return Transpose(Cofactor(mat));
	}

	Mat2 Inverse(const Mat2& mat)
	{
		float det = Determinant(mat);
		if (CMP(det, 0.0f)) 
		{
			return Mat2(); 
		}

		return Adjugate(mat) * (1.0f / det);
	}

	Mat3 Inverse(const Mat3& mat)
	{
		float det = Determinant(mat);
		if (CMP(det, 0.0f)) 
		{
			return Mat3(); 
		}

		return Adjugate(mat) * (1.0f / det);
	}

	Mat4 Inverse(const Mat4& mat)
	{
		float det = Determinant(mat);
		if (CMP(det, 0.0f)) 
		{
			return Mat4(); 
		}

		return Adjugate(mat) * (1.0f / det);
	}

	Mat4 Translation(float x, float y, float z)
	{
		Mat4 resutl;
		resutl._41 = x;
		resutl._42 = y;
		resutl._43 = z;
		return resutl;
	}

	Mat4 Translation(const Vec3& pos)
	{
		Mat4 result;
		result._41 = pos.x;
		result._42 = pos.y;
		result._43 = pos.z;
		return result;
	}

	Vec3 GetTranslation(const Mat4& mat)
	{
		return Vec3(mat._41, mat._42, mat._43);
	}

	Mat4 Scale(float x, float y, float z)
	{
		Mat4 result;
		result._11 = x;
		result._22 = y;
		result._33 = z;
		return result;
	}

	Mat4 Scale(const Vec3& vec)
	{
		Mat4 result;
		result._11 = vec.x;
		result._22 = vec.y;
		result._33 = vec.z;
		return result;
	}

	Vec3 GetScale(const Mat4& mat)
	{
		return Vec3(mat._11, mat._22, mat._33);
	}

	Mat4 Rotation(float pitch, float yaw, float roll)
	{
		return ZRotation(roll) *
			XRotation(pitch) *
			YRotation(yaw);
	}

	Mat3 Rotation3x3(float pitch, float yaw, float roll)
	{
		return YRotation3x3(yaw) *
			XRotation3x3(pitch) *
			ZRotation3x3(roll);
	}

	Mat4 ZRotation(float angle)
	{
		angle = DEG2RAD(angle);
		float c = COS(angle);
		float s = SIN(angle);

		return Mat4(
			c,    s,    0.0f, 0.0f,
			-s,   c,    0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Mat3 ZRotation3x3(float angle)
	{
		angle = DEG2RAD(angle);
		float c = COS(angle);
		float s = SIN(angle);

		return Mat3(
			c,    s,    0.0f,
			-s,   c,    0.0f,
			0.0f, 0.0f, 1.0f
		);
	}

	Mat4 XRotation(float angle)
	{
		angle = DEG2RAD(angle);
		float c = COS(angle);
		float s = SIN(angle);

		return Mat4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, c,    s,    0.0f,
			0.0f, -s,   c,    0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
			);
	}
	Mat3 XRotation3x3(float angle)
	{
		angle = DEG2RAD(angle);
		float c = COS(angle);
		float s = SIN(angle);

		return Mat3(
			1.0f, 0.0f, 0.0f,
			0.0f, c,    s,
			0.0f, -s,   c
			);
	}

	Mat4 YRotation(float angle)
	{
		angle = DEG2RAD(angle);
		float c = COS(angle);
		float s = SIN(angle);

		return Mat4(
			c,    0.0f, -s,   0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			s,    0.0f, c,    0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Mat3 YRotation3x3(float angle)
	{
		angle = DEG2RAD(angle);
		float c = COS(angle);
		float s = SIN(angle);

		return Mat3(
			c,    0.0f, -s,
			0.0f, 1.0f, 0.0f,
			s,    0.0f, c
		);
	}

	Mat4 AxisAngle(const Vec3& axis, float angle)
	{
		angle = DEG2RAD(angle);
		float c = COS(angle);
		float s = SIN(angle);
		float t = 1.0f - c;

		Vec3 a = Normalized(axis);
		float x = a.x, y = a.y, z = a.z;

		return Mat4(
			t * x * x + c,     t * x * y + s * z, t * x * z - s * y, 0.0f,
			t * x * y - s * z, t * y * y + c,     t * y * z + s * x, 0.0f,
			t * x * z + s * y, t * y * z - s * x, t * z * z + c,     0.0f,
			0.0f,              0.0f,              0.0f,              1.0f
		);
	}

	Mat3 AxisAngle3x3(const Vec3& axis, float angle)
	{
		angle = DEG2RAD(angle);
		float c = COS(angle);
		float s = SIN(angle);
		float t = 1.0f - COS(angle);

		Vec3 a = Normalized(axis);
		float x = a.x, y = a.y, z = a.z;

		return Mat3(
			t * x * x + c,     t * x * y + s * z, t * x * z - s * y,
			t * x * y - s * z, t * y * y + c,     t * y * z + s * x,
			t * x * z + s * y, t * y * z - s * x, t * z * z + c
		);
	}

	Vec3 MultiplyMat4Point(const Mat4& mat, const Vec3& vec)
	{
		return Vec3(
			mat._11 * vec.x + mat._12 * vec.y + mat._13 * vec.z + mat._14,
			mat._21 * vec.x + mat._22 * vec.y + mat._23 * vec.z + mat._24,
			mat._31 * vec.x + mat._32 * vec.y + mat._33 * vec.z + mat._34
		);

	}

	Vec3 MultiplyMat4Vec3(const Mat4& mat, const Vec3& vec)
	{
		return Vec3(
			mat._11 * vec.x + mat._12 * vec.y + mat._13 * vec.z,
			mat._21 * vec.x + mat._22 * vec.y + mat._23 * vec.z,
			mat._31 * vec.x + mat._32 * vec.y + mat._33 * vec.z
		);
	}

	Vec3 MultiplyMat3Vec3(const Mat3& mat, const Vec3& vec)
	{
		return Vec3(
			mat._11 * vec.x + mat._12 * vec.y + mat._13 * vec.z,
			mat._21 * vec.x + mat._22 * vec.y + mat._23 * vec.z,
			mat._31 * vec.x + mat._32 * vec.y + mat._33 * vec.z
		);
	}

	Mat4 Transform(const Vec3& scale, const Vec3& eulerRot,
		const Vec3& translate)
	{
		return Translation(translate) *
			Rotation(eulerRot.x, eulerRot.y, eulerRot.z) *
			Scale(scale);
	}

	Mat4 Transform(const Vec3& scale, const Vec3& rotationAxis,
		float rotationAngle, const Vec3& translate)
	{
		return Scale(scale) *
			AxisAngle(rotationAxis, rotationAngle) *
			Translation(translate);
	}

	Mat4 LookAt(const Vec3& position, const Vec3& target, const Vec3& up)
	{
		Vec3 forward = Normalized(target - position);
		Vec3 right = Normalized(Cross(forward, up));
		Vec3 newUp = Cross(right, forward);

		return Mat4(
			right.x, newUp.x, -forward.x, 0,
			right.y, newUp.y, -forward.y, 0,
			right.z, newUp.z, -forward.z, 0,
			-Dot(right, position),
			-Dot(newUp, position),
			Dot(forward, position),
			1);
	}

	Mat4 Projection(float fov, float aspect,
		float zNear, float zFar)
	{
		float tanHalfFov = TAN(DEG2RAD(fov * 0.5f));

		Mat4 result;
		result._11 = 1.0f / (aspect * tanHalfFov);
		result._22 = 1.0f / tanHalfFov;
		result._33 = -(zFar + zNear) / (zFar - zNear);
		result._34 = -1.0f;
		result._43 = -(2.0f * zFar * zNear) / (zFar - zNear);
		result._44 = 0.0f;

		return result;
	}

	Mat4 Ortho(float left, float right, float bottom,
		float top, float zNear, float zFar)
	{
		return Mat4(
			2.0f / (right - left), 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / (zFar - zNear), 0.0f,
			-(right + left) / (right - left), 
			-(top + bottom) / (top - bottom), 
			-(zFar + zNear) / (zFar - zNear), 
			1.0f
		);
	}
}