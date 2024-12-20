#pragma once

/**
 * 禁止lua持有c++内存
 */


/**
 * @brief lua对象持有的userdata对象
 * 
 */
class CXXObjectRef
{
public:
    CXXObjectRef();
    virtual ~CXXObjectRef();


private:
    void AddRef();
    void DelRef();

private:
    int m_ref_count{0};
};